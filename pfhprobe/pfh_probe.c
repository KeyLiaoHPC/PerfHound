/*
 * =================================================================================
 * VarTect - Detecting and analyzing performance variation in parallel program
 * 
 * Copyright (C) 2020 Key Liao (Liao Qiucheng)
 * 
 * This program is free software: you can redistribute it and/or modify it under the
 *  terms of the GNU General Public License as published by the Free Software 
 * Foundation, either version 3 of the License, or (at your option) any later 
 * version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with 
 * this program. If not, see https://www.gnu.org/licenses/.
 * 
 * =================================================================================
 * varapi.c
 * Description: API for vartect
 * Author: Key Liao
 * Modified: May. 28th, 2020
 * Email: keyliaohpc@gmail.com
 * =================================================================================
 */
//==================================================================================

/* Comment Syntax */
/*
 * Long block document.
 */
/* Beginning remark for functioning block/procedure. */
// Short comment for variables, short explanation.

//==================================================================================

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

#include "pfhprobe.h"
#include "pfhprobe_core.h"

static uint32_t buf_nbyte, buf_nrec; // size and # of data buffered in ram.
static uint32_t pfh_irec;            // Index for counter readings.
static int pfh_ready;

rec_t *pfh_precs; // raw data.
proc_t pfh_pinfo;
int pfh_nevt;

extern int pfh_io_init(char *root);
extern int pfh_io_mkhost();
extern int pfh_io_mkrec(char *rec_path);
extern int pfh_io_wtctag(uint32_t gid, uint32_t pid, char *tagstr);
extern int pfh_io_wtetag(int id, const char *evtstr, uint64_t evcode);
extern int pfh_io_wtrankmap(proc_t *pinfo);
extern int pfh_io_wtrec(int nrec, int nev);
extern int pfh_io_wtinfo();

/**
 * Initializing varapi
 * Directory tree:
 * pfh_root/ ------- run_#/ ------------ host/ -------- rank_#.csv 
 *                   run_info.csv        ctags.csv      
 *                                       etags.csv
 *                                       rankmap.csv
 */
int
pfh_init(char *path) {
    // User-defined data root and project name.
    char root[PATH_MAX]; // data root path, hostname.
    int i, err = 0;

#ifdef USE_MPI
    // Get process information.
    uint32_t vt_iogrp_grank[_RANK_PER_IO];
    uint32_t vt_iogrp_gcpu[_RANK_PER_IO];
    vt_get_rank(&nrank, &vt_myrank);
    /* Notice user for vartect environment */
    if (vt_myrank == 0) {
        printf("*** [Pfh-Probe] You are running in Vartect environment. "
               "Your code has been compiled with Pfh-Probe-MPI. \n");
    }
#else
    printf("*** [Pfh-Probe] Pfh-Probe is initializing. \n");
    fflush(stdout);
    pfh_pinfo.nrank = 1;
    pfh_pinfo.rank = 0;
    pfh_pinfo.cpu = sched_getcpu();
    pfh_pinfo.head = 0;
    pfh_pinfo.iorank = 0;
    gethostname(pfh_pinfo.host, _HOST_MAX);
#endif

    /* Gnerate path. */
    if (path == NULL) {
        sprintf(root, "./pfh_data");
    } else {
        // TODO: for now, no path syntax check here.
        strcpy(root, path);
    }

    /* Initializing run directory tree */
    printf("*** [Pfh-Probe] Creating data directory tree. \n");
    fflush(stdout);   
    if (pfh_pinfo.rank == 0) {
        err = pfh_io_init(root);
        if (err) {
            printf("*** [Pfh-Probe] EXIT %d. Failed to create directory tree.\n", err);
            fflush(stdout);
            exit(1);
        }
    }

    /* Initializing host directory tree */
    err = pfh_io_mkhost();
    if (err) {
        printf("*** [Pfh-Probe] EXIT %d. Failed to create host directory.\n", err);
        fflush(stdout);
        exit(1);
    }


#ifdef USE_MPI
    vt_world_barrier();
    // Get and sync process information. Print process map in the root of project.
    // <projpath>/run<run_id>_rankmap.csv
    // 
    err = vt_sync_mpi_info(projpath, &vt_run_id, &vt_head, &vt_iorank,
                             &vt_iogrp_nrank, vt_iogrp_grank, vt_iogrp_gcpu);
    //if (vt_myrank == vt_iorank)
    //    printf("%d: %d\n", vt_myrank, vt_iogrp_gcpu[0]);

#else

#endif

#ifdef USE_MPI
    vt_world_barrier();
#endif


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
    printf("*** [Pfh-Probe] Buffer: %d KiB, %d Records. \n", PFH_RECBUF_KIB, buf_nrec);
    fflush(stdout);

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
    /* Initial time reading. */
#ifdef USE_MPI
    vt_world_barrier();
    if (vt_myrank == vt_head) {
        vt_log(vt_flog, "[vt_init] Time sync started.\n");
    }
    for (i = 1; i < nrank; i ++) {
        vt_get_bias(0, i);
    }
    
    if (vt_myrank == vt_head) {
        vt_log(vt_flog, "[vt_init] Time sync finished.\n");
    }
#endif
    
#ifdef USE_MPI
    //vt_world_barrier();
    vt_tsync();
#endif
    /* Recording mappings between processes and CPUs. */
    pfh_io_wtrankmap(&pfh_pinfo);

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
    pfh_read(0, 1, 0);
    return 0;
} // END: int vt_init()

/* Set system events. */
int
pfh_set_evt(const char *etag) {
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
        printf("*** [Pfh-Probe] Event %s doesn't exist or have not been supported. \n", etag);
    }
    

    if (pfh_pinfo.rank == 0) {
        pfh_io_wtetag(pfh_nevt, etag, pfh_evcodes[pfh_nevt-1]);
        
    }
#endif // END: #ifndef _N_EV
    return 0;
} // END: int vt_set_evt

/* Commit events and configure event registers. */
void
pfh_commit() {
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
pfh_set_tag(uint32_t gid, uint32_t pid, char *tagstr) {
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
    return 0;
}


/* Get and record an event reading without boundary check. */
void
pfh_fastread(uint32_t grp_id, uint32_t p_id, double uval) {
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
pfh_read(uint32_t grp_id, uint32_t p_id, double uval) {
    if (pfh_irec >= buf_nrec) {
        printf("*** [Pfh-Probe] WARNING. NREC = %d, Buffer exceeded, record overlapped \n", 
            pfh_irec);
    }
    pfh_irec = pfh_irec % buf_nrec;
    pfh_fastread(grp_id, p_id, uval);
}


void pfh_saferead(uint32_t grp_id, uint32_t p_id, double uval) {
    if (pfh_irec >= buf_nrec) {
        printf("*** [Pfh-Probe] WARNING. Buffer exceeded, record overlapped \n");
    }
    pfh_irec = pfh_irec % buf_nrec;
    pfh_fastread(grp_id, p_id, uval);
    pfh_dump(buf_nrec - PFH_BUF_NMARGIN);
}

/**
 * Dumping collected records if the number has larger than nrec. 
 * Force dumping if nrec is set to 0. 
 * The function returns directly if pfh_irec == 0.
 */
void
pfh_dump(int nrec) {
    int n;

    if (pfh_irec == 0) {
        // Return without timing.
        return;
    }

    if (nrec == 0) {
        n = pfh_irec + 1;
    } else {
        n = nrec > buf_nrec? buf_nrec: nrec;
    } 

#ifndef USE_MPI
    if (pfh_irec + 1 + PFH_BUF_NMARGIN >= n) {
        pfh_read(0, 3, 0);
        pfh_io_wtrec(n, pfh_nevt);
        pfh_read(0, 4, 0);
    }
    pfh_irec = 0;

#else
    
        
#endif

}

#ifdef USE_MPI
void
vt_strict_sync() {
    vt_tsync();
}
#endif

/* Exiting varapi */
void
pfh_finalize() {
    int err;

    printf("*** [Pfh-Probe] User invokes finalization. \n");

    pfh_read(0, 2, 0);
    printf("*** [Pfh-Probe] Writing records. \n");
    err = pfh_io_wtrec(pfh_irec, pfh_nevt);
    if (err) {
        printf("*** [Pfh-Probe] Exit %d, failed at writing reading records. \n", err);
        fflush(stdout);
    }
    printf("*** [Pfh-Probe] Writing running info. \n", err);
    err = pfh_io_wtinfo();
    if (err) {
        printf("*** [Pfh-Probe] Exit %d, failed at writing run info. \n", err);
        fflush(stdout);
    }
    
#ifdef USE_MPI
    if (vt_myrank == vt_iorank) {
        for (i = 0; i < vt_iogrp_nrank; i ++) {
            fclose(rec_file[i]);
        }
        free(rec_file);
    }
    if (vt_myrank == 0) {
        printf("*** [Pfh-Probe] Pfh-Probe Exited. \n");
        fclose(vt_flog);
    }
#else
    printf("*** [Pfh-Probe] Pfh-Probe Exited. \n");
#endif
    _pfh_fini_ts;
#ifdef USE_MPI
    vt_mpi_clean();
#endif
}
