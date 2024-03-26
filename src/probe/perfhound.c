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

#include "perfhound.h"
#include "ph_core.h"

static uint32_t buf_nbyte, buf_nrec; // size and # of data buffered in ram.
static uint32_t ph_irec;            // Index for counter readings.
static int ph_ready;

rec_t *ph_precs; // raw data.
proc_t ph_pinfo;
int ph_nev;

extern int ph_io_mkdir(char *path);
extern int ph_io_mkname(char *root);
extern int ph_io_mkfile();
extern int ph_io_mkhost();
extern int ph_io_mkrec();
extern int ph_io_wtctag(uint32_t gid, uint32_t pid, char *tagstr);
extern int ph_io_wtetag(int id, const char *evtstr, uint64_t evcode);
extern int ph_io_wtrankmap();
extern int ph_io_wtrec(int nrec);
extern int ph_io_wtinfo();

 

/* Set system events. */
int
ph_set_evt(const char *etag) {
    // Unavailable in TS mode.

    if (ph_ready) {
        printf("*** [PH-Probe] WARNING. Your setting has been committed, please set events before ph_commit. \n");
        fflush(stdout);
        return 1;
    }
    if (ph_nev > ph_nev_max) {
        printf("*** [PH-Probe] WARNING. Too many events , this event will be omitted. \n");
        fflush(stdout);
        return 2;     
    }

    uint64_t evcode = 0;

    _ph_parse_event (evcode, etag);

    if (evcode <= 0xFFFFFFFF - 1) {
        ph_evcodes[ph_nev] = evcode;
        ph_nev ++;
        printf("*** [PH-Probe] %d Event %s added, evcode=0x%x. \n", ph_nev, etag, evcode);
    } else {
        printf("*** [PH-Probe] Event %s doesn't exist or have not been supported. \n", etag);
    }
    ph_io_wtetag(ph_nev, etag, ph_evcodes[ph_nev-1]);
        
    return 0;
} // END: int vt_set_evt

/* Commit events and configure event registers. */
void
ph_commit() {
    int err;

    _ph_config_event (ph_evcodes, ph_nev);
    printf("*** [PH-Probe] %d events have been written. \n", ph_nev);

    err = ph_io_mkrec();
    if (err) {
        printf("*** [PH-Probe] EXIT %d. Failed to create record file.\n", err);
        fflush(stdout);
        exit(1);
    }
    ph_ready = 1;
    ph_irec = 0;
    ph_read(0, 1, 0);
    return;
}


/* Set counting points' tag. */
int 
ph_set_tag(uint32_t gid, uint32_t pid, char *tagstr) {
    ph_io_wtctag(gid, pid, tagstr);
    if (pid) {
        printf("*** [PH-Probe] GID:%u PID:%u TAG:%s \n",
            gid, pid, tagstr);
    } else {
        printf("*** [PH-Probe] New Group, GID:%u, TAG:%s \n", gid, tagstr);
    }
    fflush(stdout);
    return 0;
}

/**
 * @brief Reading counter.
 * @param grp_id 
 * @param p_id 
 * @param uval 
 */
void
ph_read(uint32_t grp_id, uint32_t p_id, double uval) {
    // Get tag
    ph_precs[ph_irec].ctag[0] = grp_id;
    ph_precs[ph_irec].ctag[1] = p_id;
    // Get timestamp
    _ph_read_cy (ph_precs[ph_irec].cy);
    _ph_read_ns (ph_precs[ph_irec].ns);
    // Get user-defined FP64 value. 
    ph_precs[ph_irec].uval = uval;

    // Get PMU
#ifdef PH_OPT_EV
    // EV Mode: Reading 4 event counters after the timestamp.
    _ph_read_pm_ev (ph_precs[ph_irec].ev);

#elif PH_OPT_EVX
    // EVX Mode: Reading 12(Armv8) or 8(X86-64) counters after the timestamp.
    _ph_read_pm_evx (ph_precs[ph_irec].ev);
#endif // END: #ifdef PH_RMODE_EV

    // Index up.
    ph_irec ++;

    // Check buffer.
    // Only jump to ph_dump() when the buffer have 1 empty slot
    // for preventing infinite recursion in ph_read()
    if (ph_irec+1-buf_nrec == 0) {
        ph_dump();
    }

    return;
}


/**
 * 
 * @brief  Dumping collected records if the number has larger than nrec. 
 * Force dumping if nrec is set to 0. The function returns directly if ph_irec == 0.
 */
void
ph_dump() {
    ph_read(0, 3, ph_irec+1);  // Recording writing overhead.
    ph_io_wtrec(ph_irec);
    ph_irec = 0;
    ph_read(0, 4, 0);
    
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
ph_init(char *path) {
    // User-defined data root and project name.
    char root[PATH_MAX]; // data root path, hostname.
    int err = 0;

    printf("*** [PH-Probe] PH-Probe is initializing. \n");
    fflush(stdout);
    ph_pinfo.nrank = 1;
    ph_pinfo.rank = 0;
    ph_pinfo.cpu = sched_getcpu();
    ph_pinfo.head = 0;
    ph_pinfo.iorank = 0;
    gethostname(ph_pinfo.host, _HOST_MAX);

    /* Gnerate path. */
    if (path == NULL) {
        sprintf(root, "./ph_data");
    } else {
        // TODO: for now, no path syntax check here.
        strcpy(root, path);
    }

    /* Initializing run directory tree */
    printf("*** [PH-Probe] Creating data directory tree. \n");
    err = ph_io_mkdir(root);
    if (err) {
        printf("*** [PH-Probe] EXIT %d. Failed to build data root path.\n", err);
        fflush(stdout);
        exit(1);
    }
    printf("*** [PH-Probe] Data directory: %s\n", root);
    fflush(stdout);
    
    ph_io_mkname(root);
    err = ph_io_mkfile();
    if (err) {
        printf("*** [PH-Probe] EXIT %d. Failed to create files.\n", err);
        fflush(stdout);
        exit(1);
    }

    /* Initializing host directory tree */
    err = ph_io_mkhost();
    if (err) {
        printf("*** [PH-Probe] EXIT %d. Failed to create host directory.\n", err);
        fflush(stdout);
        exit(1);
    }

    buf_nbyte = PH_OPT_BUFSIZE * 1024;
    buf_nrec = buf_nbyte / sizeof(rec_t);
    ph_precs = (rec_t *)aligned_alloc(ALIGN, buf_nbyte);
    if (ph_precs == NULL) {
        printf("*** [PH-Probe] Failed at allocating memory for counter readings. \n");
        fflush(stdout);
        exit(1);
    }
    printf("*** [PH-Probe] Buffer: %d KiB, %d Records. \n", PH_OPT_BUFSIZE, buf_nrec);
    fflush(stdout);

    /* Set all event codes to 0. */
    for (int i = 0; i < ph_nev_max; i ++) {
        ph_evcodes[i] = 0;
    }

    /* Init wall clock timer. Implenmetations vary with predefined macros. */
    ph_irec = 0;
    ph_nev = 0;
    ph_ready = 0;

    _ph_init_ts;
    _ph_init_cy;
    printf("*** [PH-Probe] Timer has been set. \n");
    fflush(stdout);
    /* Initial time reading. */
    ph_io_wtrankmap();

    ph_set_tag(0, 0, "PHGroup");
    ph_set_tag(0, 1, "PH_S_main");
    ph_set_tag(0, 2, "PH_E_main");
    ph_set_tag(0, 3, "PH_S_ph_dump");
    ph_set_tag(0, 4, "PH_S_ph_dump");
    printf("*** [PH-Probe] Directory tree initialized. \n");

    return 0;
}

/**
 * @brief Exit PerfHound. Reading final record, then dumping the buffer.
 * @param
 */
void
ph_finalize() {
    int err;

    printf("*** [PH-Probe] User invokes finalization. \n");

    ph_read(0, 2, 0);
    printf("*** [PH-Probe] Writing records. \n");
    err = ph_io_wtrec(ph_irec);
    if (err) {
        printf("*** [PH-Probe] Exit %d, failed at writing reading records. \n", err);
        fflush(stdout);
    }
    printf("*** [PH-Probe] Writing running info. \n");
    err = ph_io_wtinfo();
    if (err) {
        printf("*** [PH-Probe] Exit %d, failed at writing run info. \n", err);
        fflush(stdout);
    }
    
    printf("*** [PH-Probe] PH-Probe Exited. \n");
    _ph_fini_ts;
}
