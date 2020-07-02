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
#include <stdio.h>
#include <stdlib.h>
#include "varapi_core.h"

data_t vt_data[_MSG_BUF_N];
int next_id;    // next data id.
int vt_nrank, vt_myrank, vt_mycpu;
// path for log, ctag, etag and raw data.
char *lpath, *ctpath, *etpath, *rawpath;
#ifdef __aarch64__
uint64_t nspt;  // nanosecond per tick, only for aarch64. 
#endif
// File pointer for logging and data output
FILE *p_vtlog, *p_vtdata;

/* Initializing varapi */
int
vt_init(char *data_dir, char *proj_name) {
    next_id = 0;
    /* Init wall clock timer */
    _vt_init_ns;
    vt_mkdir(data_dir, proj_name);
    vt_touch(&p_vtlog, &p_vtdata, data_dir, proj_name);
}

/* Set counting point tag */
int
vt_set_ctag(char *ctag) {
    int vterr = 0;

    return vterr;
}

/* Set event tag */
int
vt_set_etag(char *etag, int vt_type) {
    int vterr = 0;

    return vterr;
}

/* Read cycle and nanosec */
int
vt_read_ts(char *ctag) {
    int ctag_id;
    register uint64_t cy, ns;

    _vtread_cy(cy);
    _vtread_ns(ns);
    vt_data[next_id].ctag = ctag_id;
    vt_data[next_id].etag = VT_CYCLE;
    vt_data[next_id].val = cy;
    vt_data[next_id+1].ctag = ctag_id;
    vt_data[next_id+1].etag = VT_CYCLE;
    vt_data[next_id+1].val = ns;
    next_id ++;

    if(next_id >= _MSG_BUF_N - 2) {
        vt_fopen(&p_vtdata);
        fprintf(p_vtdata, "%u,%u,%llu", ctag_id, VT_CYCLE, cy);
        fprintf(p_vtdata, "%u,%u,%llu", ctag_id, VT_NANOSEC, ns);
        vt_fclose(&p_vtdata);
    }
}


/* Getting an event reading */
int
vt_rec(char *ctag, char *etag) {
    int vterr = 0, eid;

    vt_read_ts(ctag);
    vt_geteid(etag);

    /* Check buffer health after reading */
    if (next_id >= _MSG_BUF_N - 2) {
        vterr = vt_dump(next_id);
        vt_printf(vterr, "Dumping %d records to disk.", next_id);
    }
    return vterr;
}

/* Exiting varapi */
int
vt_finalize() {
    int vterr = 0; 

    return vterr;
}