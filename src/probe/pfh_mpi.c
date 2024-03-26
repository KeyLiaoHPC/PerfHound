#define _GNU_SOURCE
#define _ISOC11_SOURCE
#define _PH_MPI

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

#include "ph_core.h"
#include "ph_mpi.h"


#define PH_PRINTF(_msg)    if(ph_pinfo.rank == 0) {  \
                                printf(_msg);          \
                                fflush(stdout);         \
                            }  

extern int ph_io_mkname(char *root);
extern int ph_io_mkdir(char *path);
extern int ph_io_mkfile();
extern int ph_io_mkrec();
extern int ph_io_wtctag(uint32_t gid, uint32_t pid, char *tagstr);
extern int ph_io_wtetag(int id, const char *evtstr, uint64_t evcode);
extern int ph_io_wtrankmap();
extern int ph_io_wtrec(int nrec);
extern int ph_io_wtinfo();

extern int ph_mpi_rank_init();
extern void ph_mpi_barrier(MPI_Comm comm);
extern int ph_mpi_mkhost();

/* Local Global */
static uint32_t buf_nbyte, buf_nrec; // size and # of data buffered in ram.
static uint32_t ph_irec;            // Index for counter readings.
static int ph_ready;

/* Global */
rec_t *ph_precs; // raw data.
proc_t ph_pinfo;
int ph_nev;






/* Set system events. */
int
phmpi_set_evt(const char *etag) {
    // We do not support redefine events for now.
    if (ph_ready) {
        if (ph_pinfo.rank == 0) {
            printf("*** [PH-Probe] WARNING. Your setting has been committed, please set events before ph_commit. \n");
            fflush(stdout);
        }
        return 1;
    }
    
    if (ph_nev >= ph_nev_max) {
        if (ph_pinfo.rank == 0) {
            printf("*** [PH-Probe] WARNING. Too many events, this event will be omitted. \n");
            fflush(stdout);
        }
        return 2;     
    }

    uint64_t evcode = 0;

    _ph_parse_event (evcode, etag);

    if (evcode <= 0xFFFFFFFF - 1) {
        ph_evcodes[ph_nev] = evcode;
        ph_nev ++;
        if (ph_pinfo.rank == 0) {
            printf("*** [PH-Probe] %d Event %s added, evcode=0x%x. \n", ph_nev, etag, evcode);
        }
    } else {
        if (ph_pinfo.rank == 0) {
            printf("*** [PH-Probe] Event %s doesn't exist or have not been supported. \n", etag);
            fflush(stdout);
        }
    }
    

    if (ph_pinfo.rank == 0) {
        ph_io_wtetag(ph_nev, etag, ph_evcodes[ph_nev-1]);
        
    }
    return 0;
} // END: int vt_set_evt

/* Commit events and configure event registers. */
void
phmpi_commit() {
    int err;
    if (ph_nev) {
        _ph_config_event (ph_evcodes, ph_nev);
        if (ph_pinfo.rank == 0) {
            printf("*** [PH-Probe] %d events have been written. \n", ph_nev);
        }
    } 

    err = ph_io_mkrec();
    if (err) {
        printf("*** [PH-Probe] EXIT %d. Failed to create record file.\n", err);
        fflush(stdout);
        exit(1);
    }
    ph_ready = 1;
    ph_irec = 0;
    ph_mpi_barrier(MPI_COMM_WORLD);
    phmpi_fastread(0, 1, 0);
    return;
}


/* Set counting points' tag. */
int 
phmpi_set_tag(uint32_t gid, uint32_t pid, char *tagstr) {
    if (ph_pinfo.rank == 0) {
        ph_io_wtctag(gid, pid, tagstr);
    }
    if (ph_pinfo.rank == 0) {
        if (pid) {
            printf("*** [PH-Probe] GID:%u PID:%u TAG:%s \n",
                gid, pid, tagstr);
        } else {
            printf("*** [PH-Probe] New Group, GID:%u, TAG:%s \n", gid, tagstr);
        }
    } 

    ph_mpi_barrier(MPI_COMM_WORLD);
    return 0;
}


/* Get and record an event reading without boundary check. */
void
phmpi_fastread(uint32_t grp_id, uint32_t p_id, double uval) {
    // uint64_t r1 = 0, r2 = 0, r3 = 0;

    // _ph_reg_save;

    ph_precs[ph_irec].ctag[0] = grp_id;
    ph_precs[ph_irec].ctag[1] = p_id;
    _ph_read_cy (ph_precs[ph_irec].cy);
    _ph_read_ns (ph_precs[ph_irec].ns);
    ph_precs[ph_irec].uval = uval;
    
    // _ph_reg_restore;

    /* Read system event */
    switch (ph_nev){
        case 1: 
            _ph_read_pm_1 (ph_precs[ph_irec].ev);
            break;
        case 2: 
            _ph_read_pm_2 (ph_precs[ph_irec].ev);
            break;
        case 3: 
            _ph_read_pm_3 (ph_precs[ph_irec].ev);
            break;
        case 4: 
            _ph_read_pm_4 (ph_precs[ph_irec].ev);
            break;
        case 5: 
            _ph_read_pm_5 (ph_precs[ph_irec].ev);
            break;
        case 6: 
            _ph_read_pm_6 (ph_precs[ph_irec].ev);
            break;
        case 7: 
            _ph_read_pm_7 (ph_precs[ph_irec].ev);
            break;
        case 8: 
            _ph_read_pm_8 (ph_precs[ph_irec].ev);
            break;
        case 9: 
            _ph_read_pm_9 (ph_precs[ph_irec].ev);
            break;
        case 10: 
            _ph_read_pm_10 (ph_precs[ph_irec].ev);
            break;
        case 11: 
            _ph_read_pm_11 (ph_precs[ph_irec].ev);
            break;
        case 12: 
            _ph_read_pm_12 (ph_precs[ph_irec].ev);
            break;
        default:
            break;
    }

#ifdef PH_OPT_PAPI
    ph_precs[ph_irec].cy = ph_precs[ph_irec].ev[0];
#endif

    ph_irec ++;
       
}

void
phmpi_read(uint32_t grp_id, uint32_t p_id, double uval) {
    if (ph_irec+1 >= buf_nrec) {
        printf("*** [PH-Probe] RANK %d WARNING. NREC = %d, auto dump now.\n", 
            ph_pinfo.rank, ph_irec);
        fflush(stdout);
        phmpi_dump();
        ph_irec = 0;
        phmpi_fastread(grp_id, p_id, uval);
    } else {
        phmpi_fastread(grp_id, p_id, uval);
        if (ph_irec+1 >= buf_nrec) {
            printf("*** [PH-Probe] RANK %d WARNING. NREC = %d, auto dump now.\n", 
                ph_pinfo.rank, ph_irec);
            fflush(stdout);
            phmpi_dump();
        }
    }
}

/**
 * Dumping collected records if the number has larger than nrec. 
 * Force dumping if nrec is set to 0. 
 * The function returns directly if ph_irec == 0.
 */
void
phmpi_dump() {

    if (ph_irec == 0) {
        // Return empty request.
        return;
    }

    if (ph_irec >= buf_nrec) {
        printf("*** [PH-Probe] RANK %d WARNING. NREC = %d, Buffer exceeded at dumping, last data will be omitted. \n", 
        ph_pinfo.rank, ph_irec);
        ph_irec = buf_nrec - 1; // Step back for recording writing time.
    }

    phmpi_fastread(0, 3, 0);
    ph_io_wtrec(ph_irec);
    ph_irec = 0;
    phmpi_fastread(0, 4, 0);
    
    return;
}

/**
 * Initializing PerfHound
 * Directory tree:
 * ph_root/ ------- run_#/ ------------ host/ -------- r#c#.csv 
 *                   run_info.csv        ctags.csv      
 *                                       etags.csv
 *                                       rankmap.csv
 */
int
phmpi_init(char *path) {
    // User-defined data root and project name.
    char root[PATH_MAX]; // data root path, hostname.
    int i, err = 0;

    /* Init basic rank information */
    err = ph_mpi_rank_init();
    if (err) {
        printf("*** [PH-Probe] EXIT. Failed to init MPI.\n");
        exit(1);
    }

    /* Grouping ranks on the same host and get group information. */
    //err = ph_mpi_host_init(&ph_pinfo);



    /* Gnerate path. */
    if (path == NULL) {
        sprintf(root, "./ph_data");
    } else {
        // TODO: for now, no path syntax check here.
        strcpy(root, path);
    }

    /* Initializing run directory tree */
    PH_PRINTF ("*** [PH-Probe] Creating data directory tree. \n");
    fflush(stdout);   
    if (ph_pinfo.rank == 0) {
        // Rank 0 first creates root directory.
        err = ph_io_mkdir(root);
        if (err) {
            printf("*** [PH-Probe] EXIT %d. Failed to build data root path.\n", err);
            fflush(stdout);
            exit(1);
        }
        printf("*** [PH-Probe] Data directory: %s\n", root);
        fflush(stdout);
    }
    // All ranks wait here.
    MPI_Barrier(MPI_COMM_WORLD);

    err = ph_io_mkname(root);
    if (err) {
        printf("*** [PH-Probe] EXIT %d. Failed to parse data root path.\n", err);
        fflush(stdout);
        exit(1);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if (ph_pinfo.rank == 0) {
        err = ph_io_mkfile();
        if (err) {
            printf("*** [PH-Probe] EXIT %d. Failed to create files.\n", err);
            fflush(stdout);
            exit(1);
        }
    }

    /* Initializing host directory tree */
    err = ph_mpi_mkhost();
    if (err) {
        printf("*** [PH-Probe] EXIT %d. Failed to create host directory.\n", err);
        fflush(stdout);
        exit(1);
    }
    ph_mpi_barrier(MPI_COMM_WORLD);


    /* Init data space. */

    buf_nbyte = PH_OPT_BUFSIZE * 1024;
    buf_nrec = buf_nbyte / sizeof(rec_t);
    ph_precs = (rec_t *)aligned_alloc(ALIGN, buf_nbyte);
    if (ph_precs == NULL) {
        printf("*** [PH-Probe] Failed at allocating memory for counter readings. \n");
        fflush(stdout);
        exit(1);
    }
    if (ph_pinfo.rank == 0) {
        printf("*** [PH-Probe] Buffer: %d KiB, %d Records. \n", PH_OPT_BUFSIZE, buf_nrec);
        fflush(stdout);
    }

    /* Set all event codes to 0. */
    for (i = 0; i < PH_NEV; i ++) {
        ph_evcodes[i] = 0;
    }

    ph_irec = 0;
    ph_nev = 0;

    /* Init wall clock timer. Implenmetations vary with predefined macros. */
    _ph_init_ts;
    _ph_init_cy;
    ph_ready = 0;

    if (ph_pinfo.rank == 0) {
        printf("*** [PH-Probe] Timer has been set. \n");
        fflush(stdout);
    }

    phmpi_set_tag(0, 0, "PHGroup");
    phmpi_set_tag(0, 1, "PH Start");
    phmpi_set_tag(0, 2, "PH End");
    phmpi_set_tag(0, 3, "PH Wt Start");
    phmpi_set_tag(0, 4, "PH Wt End");
    if (ph_pinfo.rank == 0) {
        printf("*** [PH-Probe] Directory tree initialized. \n");
    }


    ph_mpi_barrier(MPI_COMM_WORLD);
    ph_mpi_barrier(MPI_COMM_WORLD);

    return 0;
} // END: int vt_init()

/* Exiting varapi */
void
phmpi_finalize() {
    int err;

    PH_PRINTF ("*** [PH-Probe] User invokes finalization. \n");
    PH_PRINTF ("*** [PH-Probe] Waiting for all finish. \n");
    ph_mpi_barrier(MPI_COMM_WORLD);

    phmpi_read(0, 2, 0);
    PH_PRINTF ("*** [PH-Probe] Writing records. \n");
    err = ph_io_wtrec(ph_irec);
    if (err) {
        printf("*** [PH-Probe] Rank %d Exit %d, failed at writing reading records. \n", ph_pinfo.rank, err);
        fflush(stdout);
    }
    PH_PRINTF ("*** [PH-Probe] Writing running info. \n");

    if (ph_pinfo.rank == 0) {
        err = ph_io_wtinfo();
        if (err) {
            printf("*** [PH-Probe] Exit %d, failed at writing run info. \n", err);
            fflush(stdout);
        }
    }

    PH_PRINTF ("*** [PH-Probe] PH-Probe Exited. \n");
    _ph_fini_ts;

    ph_mpi_barrier(MPI_COMM_WORLD);
    free(ph_precs);

}


