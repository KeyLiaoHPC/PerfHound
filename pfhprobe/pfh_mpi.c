#define _GNU_SOURCE
#define _ISOC11_SOURCE#include <stdio.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <sched.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>
#include <errno.h>
#include <mpi.h>

#include "pfhprobe_core.h"


#define PFH_PRINTF(_msg)    if(pfh_pinfo.rank == 0) {  \
                                printf(_msg);          \
                                fflush(stdout);         \
                            }  

extern int pfh_io_mkname(char *root);
extern int pfh_io_mkfile();
extern int pfh_io_mkrec(char *rec_path);
extern int pfh_io_wtctag(uint32_t gid, uint32_t pid, char *tagstr);
extern int pfh_io_wtetag(int id, const char *evtstr, uint64_t evcode);
extern int pfh_io_wtrankmap(proc_t *pinfo);
extern int pfh_io_wtrec(int nrec, int nev);
extern int pfh_io_wtinfo();

extern int pfh_mpi_rank_init(proc_t *pinfo);
extern void pfh_mpi_barrier(MPI_Comm comm);
extern int pfh_mpi_mkhost(int myrank, int nrank);

/* Local Global */
static uint32_t buf_nbyte, buf_nrec; // size and # of data buffered in ram.
static uint32_t pfh_irec;            // Index for counter readings.
static int pfh_ready;

/* Global */
rec_t *pfh_precs; // raw data.
proc_t pfh_pinfo;
int pfh_nevt;



/**
 * Initializing varapi
 * Directory tree:
 * pfh_root/ ------- run_#/ ------------ host/ -------- rank_#.csv 
 *                   run_info.csv        ctags.csv      
 *                                       etags.csv
 *                                       rankmap.csv
 */
int
pfhmpi_init(char *path) {
    // User-defined data root and project name.
    char root[PATH_MAX]; // data root path, hostname.
    int i, err = 0;

    /* Init basic rank information */
    err = pfh_mpi_rank_init(&pfh_pinfo);
    if (pfh_mpi_rank_init(&pfh_pinfo)) {
        printf("*** [Pfh-Probe] EXIT. Failed to init MPI.\n");
        exit(1);
    }

    /* Grouping ranks on the same host and get group information. */
    //err = pfh_mpi_host_init(&pfh_pinfo);



    /* Gnerate path. */
    if (path == NULL) {
        sprintf(root, "./pfh_data");
    } else {
        // TODO: for now, no path syntax check here.
        strcpy(root, path);
    }

    /* Initializing run directory tree */
    PFH_PRINTF ("*** [Pfh-Probe] Creating data directory tree. \n");
    fflush(stdout);   
    err = pfh_io_mkname(root);
    if (err) {
        printf("*** [Pfh-Probe] EXIT %d. Failed to parse data root path.\n", err);
        fflush(stdout);
        exit(1);
    }
    if (pfh_pinfo.rank == 0) {
        err = pfh_io_mkfile();
        if (err) {
            printf("*** [Pfh-Probe] EXIT %d. Failed to create files.\n", err);
            fflush(stdout);
            exit(1);
        }
    }

    /* Initializing host directory tree */
    err = pfh_mpi_mkhost(pfh_pinfo.rank, pfh_pinfo.nrank);
    if (err) {
        printf("*** [Pfh-Probe] EXIT %d. Failed to create host directory.\n", err);
        fflush(stdout);
        exit(1);
    }
    pfh_mpi_barrier(MPI_COMM_WORLD);


    /* Init data space. */
#ifdef USE_MPI
    vt_world_barrier();
    vt_newtype();
#endif 
    buf_nbyte = PFH_RECBUF_KIB * 1024;
    buf_nrec = buf_nbyte / sizeof(rec_t);
    pfh_precs = (rec_t *)aligned_alloc(ALIGN, buf_nbyte);
    if (pfh_precs == NULL) {
        printf("*** [Pfh-Probe] Failed at allocating memory for counter readings. \n");
        fflush(stdout);
        exit(1);
    }
    if (pfh_pinfo.rank == 0) {
        printf("*** [Pfh-Probe] Buffer: %d KiB, %d Records. \n", PFH_RECBUF_KIB, buf_nrec);
        fflush(stdout);
    }

    /* Set all event codes to 0. */
#ifdef _N_EV
    for (i = 0; i < 12; i ++) {
        pfh_evcodes[i] = 0;
    }
#endif

    /* Init wall clock timer. Implenmetations vary with predefined macros. */
#ifdef __x86_64__
    _pfh_init_ts;
#endif
    if (pfh_pinfo.rank == 0) {
        printf("*** [Pfh-Probe] Timer has been set. \n");
        fflush(stdout);
    }

    pfh_set_tag(0, 0, "PFHGroup");
    pfh_set_tag(0, 1, "PFH Start");
    pfh_set_tag(0, 2, "PFH End");
    pfh_set_tag(0, 3, "PFH Wt Start");
    pfh_set_tag(0, 4, "PFH Wt End");
    pfh_ready = 0;
    if (pfh_pinfo.rank == 0) {
        printf("*** [Pfh-Probe] Directory tree initialized. \n");
    }

#ifdef __aarch64__
    _pfh_cy_init;
#endif
    pfh_irec = 0;
    pfh_nevt = 0;

    pfh_mpi_barrier(MPI_COMM_WORLD);
    pfh_mpi_barrier(MPI_COMM_WORLD);
    pfh_read(0, 1, 0);

    return 0;
} // END: int vt_init()

/* Set system events. */
int
pfhmpi_set_evt(const char *etag) {
    // Unavailable in TS dode.
#ifndef _N_EV
    if (pfh_pinfo.rank == 0) {
        printf("*** [Pfh-Probe] WARNING. Performance event is unavailable in TS mode. Your setting wiil be omitted. \n");
        fflush(stdout);
    }
    return 0;

#else
    // We do not support redefine events for now.
    if (pfh_ready) {
        if (pfh_pinfo.rank == 0) {
            printf("*** [Pfh-Probe] WARNING. Your setting has been committed, please set events before pfh_commit. \n");
            fflush(stdout);
        }
        return 1;
    }
    
    if (pfh_nevt >= _N_EV) {
        if (pfh_pinfo.rank == 0) {
            printf("*** [Pfh-Probe] WARNING. Too many events (Max: %d), this event will be omitted. \n", _N_EV);
            fflush(stdout);
        }
        return 2;     
    }

    uint64_t evcode = 0;

    _pfh_parse_event (evcode, etag);

    if (evcode <= 0xFFFFFFFF - 1) {
        pfh_evcodes[pfh_nevt] = evcode;
        if (pfh_pinfo.rank == 0) {
            printf("*** [Pfh-Probe] Event %s added, evcode=0x%x. \n", etag, evcode);
        }
        pfh_nevt ++;
    } else {
        if (pfh_pinfo.rank == 0) {
            printf("*** [Pfh-Probe] Event %s doesn't exist or have not been supported. \n", etag);
            fflush(stdout);
        }
    }
    

    if (pfh_pinfo.rank == 0) {
        pfh_io_wtetag(pfh_nevt, etag, pfh_evcodes[pfh_nevt-1]);
        
    }
#endif // END: #ifndef _N_EV
    return 0;
} // END: int vt_set_evt

/* Commit events and configure event registers. */
void
pfhmpi_commit() {
    int err;
#ifdef _N_EV
    
    _pfh_config_event (pfh_evcodes, pfh_nevt);
    if (pfh_pinfo.rank == 0) {
        printf("*** [Pfh-Probe] %d events have been written. \n", pfh_nevt);
    }
#endif
#ifdef USE_MPI
    pfh_mpi_barrier();
#endif
    err = pfh_io_mkrec(NULL);
    if (err) {
        printf("*** [Pfh-Probe] EXIT %d. Failed to create record file.\n", err);
        fflush(stdout);
        exit(1);
    }
    pfh_ready = 1;
    pfh_read(0, 1, 0);
    return;
}


/* Set counting points' tag. */
int 
pfhmpi_set_tag(uint32_t gid, uint32_t pid, char *tagstr) {
    if (pfh_pinfo.rank == 0) {
        pfh_io_wtctag(gid, pid, tagstr);
    }
    if (pfh_pinfo.rank == 0) {
        if (pid) {
            printf("*** [Pfh-Probe] GID:%u PID:%u TAG:%s \n",
                gid, pid, tagstr);
        } else {
            printf("*** [Pfh-Probe] New Group, GID:%u, TAG:%s \n", gid, tagstr);
        }
    } 

    pfh_mpi_barrier(MPI_COMM_WORLD);
    return 0;
}


/* Get and record an event reading without boundary check. */
void
pfhmpi_fastread(uint32_t grp_id, uint32_t p_id, double uval) {
    // uint64_t r1 = 0, r2 = 0, r3 = 0;

    // _pfh_reg_save;

    pfh_precs[pfh_irec].ctag[0] = grp_id;
    pfh_precs[pfh_irec].ctag[1] = p_id;
    _pfh_read_cy (pfh_precs[pfh_irec].cy);
    _pfh_read_ns (pfh_precs[pfh_irec].ns);
    pfh_precs[pfh_irec].uval = uval;
    
    // _pfh_reg_restore;

    /* Read system event */
#ifdef _N_EV
    switch (pfh_nevt){
        case 1: 
            _pfh_read_pm_1 (pfh_precs[pfh_irec].ev);
            break;
        case 2: 
            _pfh_read_pm_2 (pfh_precs[pfh_irec].ev);
            break;
        case 3: 
            _pfh_read_pm_3 (pfh_precs[pfh_irec].ev);
            break;
        case 4: 
            _pfh_read_pm_4 (pfh_precs[pfh_irec].ev);
            break;
        case 5: 
            _pfh_read_pm_5 (pfh_precs[pfh_irec].ev);
            break;
        case 6: 
            _pfh_read_pm_6 (pfh_precs[pfh_irec].ev);
            break;
        case 7: 
            _pfh_read_pm_7 (pfh_precs[pfh_irec].ev);
            break;
        case 8: 
            _pfh_read_pm_8 (pfh_precs[pfh_irec].ev);
            break;
#ifdef __aarch64__
        case 9: 
            _pfh_read_pm_9 (pfh_precs[pfh_irec].ev);
            break;
        case 10: 
            _pfh_read_pm_10 (pfh_precs[pfh_irec].ev);
            break;
        case 11: 
            _pfh_read_pm_11 (pfh_precs[pfh_irec].ev);
            break;
        case 12: 
            _pfh_read_pm_12 (pfh_precs[pfh_irec].ev);
            break;
#endif // END: #ifdef __aarch64__
        default:
            break;
    }
#endif

    pfh_irec ++;
       
}

void
pfhmpi_read(uint32_t grp_id, uint32_t p_id, double uval) {
    if (pfh_irec >= buf_nrec) {
        printf("*** [Pfh-Probe] RANK %d WARNING. NREC = %d, Buffer exceeded, record overlapped \n", 
        pfh_pinfo.rank, pfh_irec);
        fflush(stdout);
        pfh_irec = 0;
    }
    pfh_fastread(grp_id, p_id, uval);
}


void 
pfhmpi_saferead(uint32_t grp_id, uint32_t p_id, double uval) {
    pfh_fastread(grp_id, p_id, uval);

    if (pfh_irec == buf_nrec - 1) {
        printf("*** [Pfh-Probe] Rank %d: Auto dumping. \n", pfh_pinfo.rank);
    }
    pfh_dump();
}

/**
 * Dumping collected records if the number has larger than nrec. 
 * Force dumping if nrec is set to 0. 
 * The function returns directly if pfh_irec == 0.
 */
void
pfhmpi_dump() {
    int n;

    if (pfh_irec == 0) {
        // Return without timing.
        return;
    }

    if (pfh_irec == buf_nrec) {
        printf("*** [Pfh-Probe] RANK %d WARNING. NREC = %d, Buffer exceeded at dumping, last data will be omitted. \n", 
        pfh_pinfo.rank, pfh_irec);
        pfh_irec --; // Step back for recording writing time.
    }

    pfh_fastread(0, 3, 0);
    pfh_io_wtrec(pfh_irec, pfh_nevt);
    pfh_irec = 0;
    pfh_fastread(0, 4, 0);
    
    return;
}

/* Exiting varapi */
void
pfhmpi_finalize() {
    int err;

    PFH_PRINTF ("*** [Pfh-Probe] User invokes finalization. \n");

    pfh_read(0, 2, 0);
    PFH_PRINTF ("*** [Pfh-Probe] Writing records. \n");
    err = pfh_io_wtrec(pfh_irec, pfh_nevt);
    if (err) {
        printf("*** [Pfh-Probe] Rank %d Exit %d, failed at writing reading records. \n", pfh_pinfo.rank, err);
        fflush(stdout);
    }
    PFH_PRINTF ("*** [Pfh-Probe] Writing running info. \n", err);

    if (pfh_pinfo.rank == 0) {
        err = pfh_io_wtinfo();
        if (err) {
            printf("*** [Pfh-Probe] Exit %d, failed at writing run info. \n", err);
            fflush(stdout);
        }
    }

    PFH_PRINTF ("*** [Pfh-Probe] Pfh-Probe Exited. \n");
    _pfh_fini_ts;

    pfh_mpi_barrier(MPI_COMM_WORLD);
    free(pfh_precs);

}


