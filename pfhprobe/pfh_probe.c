/*
 * =================================================================================
 * PerfHound - Detecting and analyzing performance variation in parallel program
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
 * Description: API for PerfHound
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
int pfh_nev;

extern int pfh_io_mkname(char *root);
extern int pfh_io_mkfile();
extern int pfh_io_mkhost();
extern int pfh_io_mkrec();
extern int pfh_io_wtctag(uint32_t gid, uint32_t pid, char *tagstr);
extern int pfh_io_wtetag(int id, const char *evtstr, uint64_t evcode);
extern int pfh_io_wtrankmap();
extern int pfh_io_wtrec(int nrec);
extern int pfh_io_wtinfo();

 

/* Set system events. */
int
pfh_set_evt(const char *etag) {
    // Unavailable in TS dode.

    // We do not support redefine events for now.
    if (pfh_ready) {
        printf("*** [Pfh-Probe] WARNING. Your setting has been committed, please set events before pfh_commit. \n");
        fflush(stdout);
        return 1;
    }
    if (pfh_nev > pfh_nev_max) {
        printf("*** [Pfh-Probe] WARNING. Too many events , this event will be omitted. \n");
        fflush(stdout);
        return 2;     
    }

    uint64_t evcode = 0;

    _pfh_parse_event (evcode, etag);

    if (evcode <= 0xFFFFFFFF - 1) {
        pfh_evcodes[pfh_nev] = evcode;
        pfh_nev ++;
        printf("*** [Pfh-Probe] %d Event %s added, evcode=0x%x. \n", pfh_nev, etag, evcode);
    } else {
        printf("*** [Pfh-Probe] Event %s doesn't exist or have not been supported. \n", etag);
    }
    pfh_io_wtetag(pfh_nev, etag, pfh_evcodes[pfh_nev-1]);
        
    return 0;
} // END: int vt_set_evt

/* Commit events and configure event registers. */
void
pfh_commit() {
    int err;

    _pfh_config_event (pfh_evcodes, pfh_nev);
    printf("*** [Pfh-Probe] %d events have been written. \n", pfh_nev);

    err = pfh_io_mkrec();
    if (err) {
        printf("*** [Pfh-Probe] EXIT %d. Failed to create record file.\n", err);
        fflush(stdout);
        exit(1);
    }
    pfh_ready = 1;
    pfh_irec = 0;
    pfh_fastread(0, 1, 0);
    return;
}


/* Set counting points' tag. */
int 
pfh_set_tag(uint32_t gid, uint32_t pid, char *tagstr) {
    pfh_io_wtctag(gid, pid, tagstr);
    if (pid) {
        printf("*** [Pfh-Probe] GID:%u PID:%u TAG:%s \n",
            gid, pid, tagstr);
    } else {
        printf("*** [Pfh-Probe] New Group, GID:%u, TAG:%s \n", gid, tagstr);
    }
    fflush(stdout);
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
    switch (pfh_nev){
        case 0:
            break;
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
pfh_read(uint32_t grp_id, uint32_t p_id, double uval) {
    if (pfh_irec+1 >= buf_nrec) {
        printf("*** [Pfh-Probe] RANK %d WARNING. NREC = %d, auto dump now.\n", 
            pfh_pinfo.rank, pfh_irec);
        fflush(stdout);
        pfh_dump();
        pfh_irec = 0;
        pfh_fastread(grp_id, p_id, uval);
    } else {
        pfh_fastread(grp_id, p_id, uval);
        if (pfh_irec+1 >= buf_nrec) {
            printf("*** [Pfh-Probe] RANK %d WARNING. NREC = %d, auto dump now.\n", 
                pfh_pinfo.rank, pfh_irec);
            fflush(stdout);
            pfh_dump();
        }
    }
}


/**
 * Dumping collected records if the number has larger than nrec. 
 * Force dumping if nrec is set to 0. 
 * The function returns directly if pfh_irec == 0.
 */
void
pfh_dump() {

    if (pfh_irec == 0) {
        // Return without timing.
        return;
    }

    if (pfh_irec == buf_nrec) {
        printf("*** [Pfh-Probe] RANK %d WARNING. NREC = %d, Buffer exceeded at dumping, last data will be omitted. \n", 
        pfh_pinfo.rank, pfh_irec);
        fflush(stdout);
        pfh_irec --; // Step back for recording writing time.
    }

    pfh_fastread(0, 3, 0);
    pfh_io_wtrec(pfh_irec);
    pfh_irec = 0;
    pfh_fastread(0, 4, 0);
    
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
pfh_init(char *path) {
    // User-defined data root and project name.
    char root[PATH_MAX]; // data root path, hostname.
    int err = 0;

    printf("*** [Pfh-Probe] Pfh-Probe is initializing. \n");
    fflush(stdout);
    pfh_pinfo.nrank = 1;
    pfh_pinfo.rank = 0;
    pfh_pinfo.cpu = sched_getcpu();
    pfh_pinfo.head = 0;
    pfh_pinfo.iorank = 0;
    gethostname(pfh_pinfo.host, _HOST_MAX);

    /* Gnerate path. */
    if (path == NULL) {
        sprintf(root, "./pfh_data");
    } else {
        // TODO: for now, no path syntax check here.
        strcpy(root, path);
    }

    /* Initializing run directory tree */
    printf("*** [Pfh-Probe] Creating data directory tree. \n");
    pfh_io_mkname(root);
    err = pfh_io_mkdir(root);
    if (err) {
        printf("*** [Pfh-Probe] EXIT %d. Failed to build data root path.\n", err);
        fflush(stdout);
        exit(1);
    }
    printf("*** [Pfh-Probe] Data directory: %s\n", root);
    fflush(stdout);
    err = pfh_io_mkfile();
    if (err) {
        printf("*** [Pfh-Probe] EXIT %d. Failed to build data root path.\n", err);
        fflush(stdout);
        exit(1);
    }
    printf("*** [Pfh-Probe] Data directory: %s\n", root);
    fflush(stdout);

    pfh_io_mkname(root);
    err = pfh_io_mkfile();
    if (err) {
        printf("*** [Pfh-Probe] EXIT %d. Failed to create files.\n", err);
        fflush(stdout);
        exit(1);
    }

    /* Initializing host directory tree */
    err = pfh_io_mkhost();
    if (err) {
        printf("*** [Pfh-Probe] EXIT %d. Failed to create host directory.\n", err);
        fflush(stdout);
        exit(1);
    }

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
    for (int i = 0; i < 12; i ++) {
        pfh_evcodes[i] = 0;
    }

    /* Init wall clock timer. Implenmetations vary with predefined macros. */
    pfh_irec = 0;
    pfh_nev = 0;
    pfh_ready = 0;
#ifdef __x86_64__
    pfh_nev_max = pfh_nev_max > 8 ? 8 : pfh_nev_max;
#endif
    _pfh_init_ts;
    _pfh_init_cy;
    printf("*** [Pfh-Probe] Timer has been set. \n");
    fflush(stdout);
    /* Initial time reading. */
    pfh_io_wtrankmap();

    pfh_set_tag(0, 0, "PFHGroup");
    pfh_set_tag(0, 1, "PFH Start");
    pfh_set_tag(0, 2, "PFH End");
    pfh_set_tag(0, 3, "PFH Wt Start");
    pfh_set_tag(0, 4, "PFH Wt End");
    printf("*** [Pfh-Probe] Directory tree initialized. \n");


    return 0;
}

/* Exiting varapi */
void
pfh_finalize() {
    int err;

    printf("*** [Pfh-Probe] User invokes finalization. \n");

    pfh_read(0, 2, 0);
    printf("*** [Pfh-Probe] Writing records. \n");
    err = pfh_io_wtrec(pfh_irec);
    if (err) {
        printf("*** [Pfh-Probe] Exit %d, failed at writing reading records. \n", err);
        fflush(stdout);
    }
    printf("*** [Pfh-Probe] Writing running info. \n");
    err = pfh_io_wtinfo();
    if (err) {
        printf("*** [Pfh-Probe] Exit %d, failed at writing run info. \n", err);
        fflush(stdout);
    }
    
    printf("*** [Pfh-Probe] Pfh-Probe Exited. \n");
    _pfh_fini_ts;
}
