#define _GNU_SOURCE
#define _ISOC11_SOURCE

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
#include "pfh_mpi.h"


#define PFH_PRINTF(_msg)    if(pfh_pinfo.rank == 0) {  \
                                printf(_msg);          \
                                fflush(stdout);         \
                            }  

extern int pfh_io_mkname(char *root);
extern int pfh_io_mkdir(char *path);
extern int pfh_io_mkfile();
extern int pfh_io_mkrec();
extern int pfh_io_wtctag(uint32_t gid, uint32_t pid, char *tagstr);
extern int pfh_io_wtetag(int id, const char *evtstr, uint64_t evcode);
extern int pfh_io_wtrankmap();
extern int pfh_io_wtrec(int nrec);
extern int pfh_io_wtinfo();

extern int pfh_mpi_rank_init();
extern void pfh_mpi_barrier(MPI_Comm comm);
extern int pfh_mpi_mkhost();

/* Local Global */
static uint32_t buf_nbyte, buf_nrec; // size and # of data buffered in ram.
static uint32_t pfh_irec;            // Index for counter readings.
static int pfh_ready;

/* Global */
rec_t *pfh_precs; // raw data.
proc_t pfh_pinfo;
int pfh_nev;






/* Set system events. */
int
pfhmpi_set_evt(const char *etag) {
    // We do not support redefine events for now.
    if (pfh_ready) {
        if (pfh_pinfo.rank == 0) {
            printf("*** [Pfh-Probe] WARNING. Your setting has been committed, please set events before pfh_commit. \n");
            fflush(stdout);
        }
        return 1;
    }
    
    if (pfh_nev >= pfh_nev_max) {
        if (pfh_pinfo.rank == 0) {
            printf("*** [Pfh-Probe] WARNING. Too many events, this event will be omitted. \n");
            fflush(stdout);
        }
        return 2;     
    }

    uint64_t evcode = 0;

    _pfh_parse_event (evcode, etag);

    if (evcode <= 0xFFFFFFFF - 1) {
        pfh_evcodes[pfh_nev] = evcode;
        pfh_nev ++;
        if (pfh_pinfo.rank == 0) {
            printf("*** [Pfh-Probe] %d Event %s added, evcode=0x%x. \n", pfh_nev, etag, evcode);
        }
    } else {
        if (pfh_pinfo.rank == 0) {
            printf("*** [Pfh-Probe] Event %s doesn't exist or have not been supported. \n", etag);
            fflush(stdout);
        }
    }
    

    if (pfh_pinfo.rank == 0) {
        pfh_io_wtetag(pfh_nev, etag, pfh_evcodes[pfh_nev-1]);
        
    }
    return 0;
} // END: int vt_set_evt

/* Commit events and configure event registers. */
void
pfhmpi_commit() {
    int err;
    if (pfh_nev) {
        _pfh_config_event (pfh_evcodes, pfh_nev);
        if (pfh_pinfo.rank == 0) {
            printf("*** [Pfh-Probe] %d events have been written. \n", pfh_nev);
        }
    } 

    err = pfh_io_mkrec();
    if (err) {
        printf("*** [Pfh-Probe] EXIT %d. Failed to create record file.\n", err);
        fflush(stdout);
        exit(1);
    }
    pfh_ready = 1;
    pfh_irec = 0;
    pfh_mpi_barrier(MPI_COMM_WORLD);
    pfhmpi_fastread(0, 1, 0);
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
    switch (pfh_nev){
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
        default:
            break;
    }

#ifdef USE_PAPI
    pfh_precs[pfh_irec].cy = pfh_precs[pfh_irec].ev[0];
#endif

    pfh_irec ++;
       
}

void
pfhmpi_read(uint32_t grp_id, uint32_t p_id, double uval) {
    if (pfh_irec+1 >= buf_nrec) {
        printf("*** [Pfh-Probe] RANK %d WARNING. NREC = %d, auto dump now.\n", 
            pfh_pinfo.rank, pfh_irec);
        fflush(stdout);
        pfhmpi_dump();
        pfh_irec = 0;
        pfhmpi_fastread(grp_id, p_id, uval);
    } else {
        pfhmpi_fastread(grp_id, p_id, uval);
        if (pfh_irec+1 >= buf_nrec) {
            printf("*** [Pfh-Probe] RANK %d WARNING. NREC = %d, auto dump now.\n", 
                pfh_pinfo.rank, pfh_irec);
            fflush(stdout);
            pfhmpi_dump();
        }
    }
}

/**
 * Dumping collected records if the number has larger than nrec. 
 * Force dumping if nrec is set to 0. 
 * The function returns directly if pfh_irec == 0.
 */
void
pfhmpi_dump() {

    if (pfh_irec == 0) {
        // Return empty request.
        return;
    }

    if (pfh_irec >= buf_nrec) {
        printf("*** [Pfh-Probe] RANK %d WARNING. NREC = %d, Buffer exceeded at dumping, last data will be omitted. \n", 
        pfh_pinfo.rank, pfh_irec);
        pfh_irec = buf_nrec - 1; // Step back for recording writing time.
    }

    pfhmpi_fastread(0, 3, 0);
    pfh_io_wtrec(pfh_irec);
    pfh_irec = 0;
    pfhmpi_fastread(0, 4, 0);
    
    return;
}

/**
 * Initializing PerfHound
 * Directory tree:
 * pfh_root/ ------- run_#/ ------------ host/ -------- r#c#.csv 
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
    err = pfh_mpi_rank_init();
    if (err) {
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
    if (pfh_pinfo.rank == 0) {
        // Rank 0 first creates root directory.
        err = pfh_io_mkdir(root);
        if (err) {
            printf("*** [Pfh-Probe] EXIT %d. Failed to build data root path.\n", err);
            fflush(stdout);
            exit(1);
        }
        printf("*** [Pfh-Probe] Data directory: %s\n", root);
        fflush(stdout);
    }
    // All ranks wait here.
    MPI_Barrier(MPI_COMM_WORLD);

    err = pfh_io_mkname(root);
    if (err) {
        printf("*** [Pfh-Probe] EXIT %d. Failed to parse data root path.\n", err);
        fflush(stdout);
        exit(1);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if (pfh_pinfo.rank == 0) {
        err = pfh_io_mkfile();
        if (err) {
            printf("*** [Pfh-Probe] EXIT %d. Failed to create files.\n", err);
            fflush(stdout);
            exit(1);
        }
    }

    /* Initializing host directory tree */
    err = pfh_mpi_mkhost();
    if (err) {
        printf("*** [Pfh-Probe] EXIT %d. Failed to create host directory.\n", err);
        fflush(stdout);
        exit(1);
    }
    pfh_mpi_barrier(MPI_COMM_WORLD);


    /* Init data space. */

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
    for (i = 0; i < 12; i ++) {
        pfh_evcodes[i] = 0;
    }

    pfh_irec = 0;
    pfh_nev = 0;

    /* Init wall clock timer. Implenmetations vary with predefined macros. */
    _pfh_init_ts;
    _pfh_init_cy;
    pfhmpi_set_evt("CPU_CLK_UNHALTED");
    pfh_ready = 0;

    if (pfh_pinfo.rank == 0) {
        printf("*** [Pfh-Probe] Timer has been set. \n");
        fflush(stdout);
    }

    pfhmpi_set_tag(0, 0, "PFHGroup");
    pfhmpi_set_tag(0, 1, "PFH Start");
    pfhmpi_set_tag(0, 2, "PFH End");
    pfhmpi_set_tag(0, 3, "PFH Wt Start");
    pfhmpi_set_tag(0, 4, "PFH Wt End");
    if (pfh_pinfo.rank == 0) {
        printf("*** [Pfh-Probe] Directory tree initialized. \n");
    }


    pfh_mpi_barrier(MPI_COMM_WORLD);
    pfh_mpi_barrier(MPI_COMM_WORLD);

    return 0;
} // END: int vt_init()

/* Exiting varapi */
void
pfhmpi_finalize() {
    int err;

    PFH_PRINTF ("*** [Pfh-Probe] User invokes finalization. \n");
    PFH_PRINTF ("*** [Pfh-Probe] Waiting for all finish. \n");
    pfh_mpi_barrier(MPI_COMM_WORLD);

    pfhmpi_read(0, 2, 0);
    PFH_PRINTF ("*** [Pfh-Probe] Writing records. \n");
    err = pfh_io_wtrec(pfh_irec);
    if (err) {
        printf("*** [Pfh-Probe] Rank %d Exit %d, failed at writing reading records. \n", pfh_pinfo.rank, err);
        fflush(stdout);
    }
    PFH_PRINTF ("*** [Pfh-Probe] Writing running info. \n");

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


