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
#include <unistd.h>
#include <linux/getcpu.h>
#include "varapi_core.h"

#ifdef USE_MPI
#include <mpi.h>
#endif

/* Variables for Varapi's control */
int vt_not_ready;
int vr_run_id;

/* Variables for files and information */
data_t vt_data[_MSG_BUF_N];                 // Varapi raw data.
int next_id;                                // Next data id.
char hostpath[PATH_MAX];                    // Data path for the host.
char cfile[PATH_MAX], efile[PATH_MAX];      // Full path of ctag and etag.
char logfile[PATH_MAX], datafile[PATH_MAX]; // Full path of log and data.
char timestr[16];                         // Timestamp records. One stamp per line.

/* Process info*/
int vt_nrank, vt_myrank, vt_mycpu, vt_mynuma;   // # of mpi ranks, rank id, core id.
int vt_is_iorank;           // IO rank flag
char vt_myhost[HOST_MAX];   // My host name
rank_t *vt_rinfo;           // Rank information list

/* Var for nanosec reading */
#if defined(__aarch64__) && defined(USE_CNTVCT)
uint64_t nspt;  // nanosecond per tick, only for aarch64. 
#else
struct timespec ts;
#endif

/* Extern file-operating functions in file_op.c*/
extern int vt_mkdir(char *data_root, char *proj_name, char *host_name);
extern int vt_touch(char *cfile, char *efile, char *logfile, char *datafile);


/* Initializing varapi */
int
vt_init(char *u_data_root, char *u_proj_name) {
    int vterr = 0;
    unsigned int cpuid, numanode;
    vt_not_ready = 1;
    next_id = 0;

    /* Get host and process info */
    getcpu(&cpuid, &numanode);
    gethostname(vt_myhost, HOST_MAX);
    vt_mycpu = cpuid;
    vt_mynuma = numanode;
    vt_is_iorank = 0;

#ifdef USE_MPI
    MPI_Comm_size(MPI_COMM_WORLD, &vt_nrank);
    MPI_Comm_rank(MPI_COMM_WORLD, &vt_myrank);
    // All-to-all gathering process information.
#else
    vt_nrank = 1;
    vt_myrank = 0;
    vt_is_iorank = 1;
#endif

    /* Initialization of output directory and file*/
    /*
     * 
     * data path: <Data_Root>/<Proj_Name>/<Host_Name>/<files>
     * data file:
     * run#_ctag.csv                    CSV file for ctag. One copy per host.
     * run#_etag.csv                    CSV file for etag. One copy per host.
     * run#_r#_c#_all.csv               CSV file for raw tracing data. One per rank.
     * varapi_run#_<host>_<tstamp>.log  Varapi log for the host. One per host.
     * 
     */ 
    vterr = vt_mkdir(u_data_root, u_proj_name, hostpath);
    if(vterr) {
        vt_not_ready = 1;
        return -1;
    }
    // TODO: only 1 process now.
    vt_getstamp(hostpath, timestr, &vt_run_id);
    if(vt_is_iorank) {
        sprintf(cfile, "%s/run%d_ctag.csv", hostpath, vt_run_id);
        sprintf(efile, "%s/run%d_etag.csv", hostpath, vt_run_id);
        sprintf(logfile, "%s/varapi_run%d_%s_%s.log", hostpath, vt_run_id, vt_myhost, timestr);
    }
    sprintf(datafile, "%s/run%d_r%d_c%d_all.csv", hostpath, vt_run_id, vt_myrank, vt_mycpu);
    vterr = vt_touch(&p_vtlog, &p_vtdata, data_dir, proj_name);
    if(vterr) {
        vt_not_ready = 1;
        return -1;
    }

    /* Init wall clock timer. Implenmetations vary with predefined macros. */
    _vt_init_ns;

    /* Initial time reading. */

}

/* Set counting point tag */
void
vt_set_ctag(char *ctag) {
    if(vt_not_ready) return; // Nothing will be executed.

    int vterr = 0;

    return vterr;
}

/* Set event tag */
void
vt_set_etag(char *etag, int vt_type) {
    if(vt_not_ready) return;

    int vterr = 0;

    return vterr;
}

/* Read cycle and nanosec */
int
vt_read_ts(char *ctag) {
    if(vt_not_ready) return; // Nothing will be executed.

    int ctag_id;
    register uint64_t cy, ns;

    _vtread_cy(cy);
    _vtread_ns(ns);
    
    /* Save to data struct arary */
    vt_data[next_id].ctag = ctag_id;
    vt_data[next_id].etag = VT_CYCLE;
    vt_data[next_id].val = cy;
    vt_data[next_id+1].ctag = ctag_id;
    vt_data[next_id+1].etag = VT_CYCLE;
    vt_data[next_id+1].val = ns;
    next_id ++;

    /* Check overflow */
    if(next_id >= _MSG_BUF_N - 2) {
        // Writing to data file
        vt_fopen(&p_vtdata);
        fprintf(p_vtdata, "%u,%u,%llu", ctag_id, VT_CYCLE, cy);
        fprintf(p_vtdata, "%u,%u,%llu", ctag_id, VT_NANOSEC, ns);
        vt_fclose(&p_vtdata);
    }
}


/* Getting an event reading */
int
vt_rec(char *ctag, char *etag) {
    if(vt_not_ready) return; // Nothing will be executed.

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
    if(vt_not_ready) return; // Nothing will be executed.

    int vterr = 0; 

    return vterr;
}