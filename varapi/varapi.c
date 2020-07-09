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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <sched.h>
#include <string.h>
#include <stdint.h>
#include "varapi.h"
#include "varapi_core.h"
#include "vt_etag.h"

#ifdef USE_MPI
#include <mpi.h>
#endif

/* Variables for Varapi's control */
int vt_run_id;

/* Variables for files and information */
//data_t vt_data[_MSG_BUF_N];                 // Varapi raw data.
char vt_data[_MSG_BUF_N][_MSG_LEN];
int next_id, vt_nmsg;                           // Next data id.
char hostpath[_PATH_MAX];                       // Data path for the host.
char projpath[_PATH_MAX];                       // Data path for the host.
char cfile[_PATH_MAX], efile[_PATH_MAX];        // Full path of ctag and etag.
char logfile[_PATH_MAX], datafile[_PATH_MAX];   // Full path of log and data.
// This timestamp is only for project records.
char timestr[16];                               // Timestamp records. One stamp per line.
FILE *fp_log, *fp_data;

/* MPI and process info*/
uint32_t vt_nrank, vt_myrank, vt_mycpu, vt_mynuma;   // # of mpi ranks, rank id, core id.
uint32_t vt_iorank;                               // IO rank flag
uint32_t vt_headrank;                              // Flag for the head rank of a host.
char vt_myhost[_HOST_MAX];                      // My host name
rank_t *vt_rinfo;                               // Rank information list

/* Var for nanosec reading */
#if defined(__aarch64__) && defined(USE_CNTVCT)
uint64_t nspt;  // nanosecond per tick, only for aarch64. 
#else
struct timespec ts;
#endif

/* Extern file-operating functions in file_op.c*/
extern int vt_mkdir(char *path);
extern int vt_touch(char *path, char *mode);
extern void vt_getstamp(char *hostpath, char *timestr, int *run_id);
extern void vt_putstamp(char *hostpath, char *timestr);
extern void vt_log(FILE *fp, char *fmt, ...);
#ifdef USE_MPI
/* Extern MPI functions */
extern void vt_get_rank(uint32_t *nrank, uint32_t *myrank);
extern int vt_sync_mpi_info(char *myhost, uint32_t nrank, uint32_t myrank, uint32_t mycpu, 
                            uint32_t *head, int *iorank, char *projpath, int run_id);
#endif
 

/* Initializing varapi */
int
vt_init(char *u_data_root, char *u_proj_name) {
    // User-defined data root and project name.
    char udroot[_PATH_MAX], upname[_PATH_MAX];
    int vterr = 0;
    next_id = 0;
    vt_nmsg = 0;

#ifdef USE_MPI
    // Get process information.
    vt_get_rank(&vt_nrank, &vt_myrank);
#else
    vt_nrank = 1;
    vt_myrank = 0;
#endif

    /* Get host and process info */
    gethostname(vt_myhost, _HOST_MAX);
    vt_mycpu = sched_getcpu();

    /* Gnerate path. */
    if (u_data_root == NULL) {
        sprintf(udroot, "./vartect_data");
    }
    else {
        // TODO: for now, no path syntax check here.
        sprintf(udroot, "%s", u_data_root);
    }
    if (u_proj_name == NULL) {
        sprintf(upname, "vartect_test");
    }
    else {
        // TODO: for now no syntax check here.
        sprintf(upname, u_proj_name);
    }
    // Path for project directory and host drectory which can be relative path for now.
    sprintf(projpath, "%s/%s", udroot, upname);
    sprintf(hostpath, "%s/%s", projpath, vt_myhost);

    // Create directory
    if (vt_myrank == 0) {
        vt_mkdir(projpath);
    }

#ifdef USE_MPI
    // Get process information.
    vt_sync_mpi_info(vt_myhost, vt_nrank, vt_myrank, vt_mycpu, &vt_headrank, &vt_iorank, 
                     projpath, vt_run_id);
    exit(0);
#else
    vt_headrank = 0;
    vt_iorank = 0;
#endif

    /* Initialization of output directory and file*/
    /*
     * 
     * data path: <Data_Root>/<Proj_Name>/<Host_Name>/<files>
     * data file:
     * run#_procmap.csv                 CSV file for process map. One copy per host.
     * run#_r#_c#_all.csv               CSV file for raw tracing data. One per rank.
     * varapi_run#_<host>_<tstamp>.log  Varapi log for the host. One per host.
     * 
     */ 


    if (vt_myrank == vt_headrank) {
        vterr = vt_mkdir(hostpath);
    }

    if (vterr) {
        printf("*** EXIT. ERROR: VARAPI init failed. ***\n");
        exit(1);
    }

    if (vt_myrank == 0) {
        vt_getstamp(hostpath, timestr, &vt_run_id);
    }
#ifdef USE_MPI
    else {

    }
#endif
    sprintf(logfile, "%s/varapi_run%d_%s_%s.log", hostpath, vt_run_id, vt_myhost, timestr);
    // TODO: only 1 process now.
    if(vt_iorank) {
        // Initialize logging, get current run id in the project
        fp_log = NULL;
        fp_log = fopen(logfile, "w");
        if(fp_log == NULL) {
            printf("*** EXIT. ERROR: VARAPI log file init failed. ***\n");
            exit(1);
        }
        vt_log(fp_log, "[vt_init] Varapi is started.\n");
    }   // END: if(vt_is_iorank)
    // Each process maintains its own data tracing file.
    sprintf(datafile, "%s/run%d_r%d_c%d_all.csv", hostpath, vt_run_id, vt_myrank, vt_mycpu);
    fp_data = NULL;
    fp_data = fopen(datafile, "w");
    if (vt_touch(datafile, "w")) {
#ifdef USE_MPI
        // When using MPI, gathering all ready flag, kick start if every process is ready.
#else
        exit(1);
#endif        
    } 
    else {
        vt_log(fp_log, "[vt_init] Rank %d on CPU # %d creates data file %s.\n", 
               vt_myrank, vt_mycpu, datafile);
    }// END: if (vt_touch(datafile, "w"))

    /* Init wall clock timer. Implenmetations vary with predefined macros. */
    _vt_init_ns;

    /* Initial time reading. */
    vt_read("VT_START", 8, 0, 0, 0);
    vt_write();
    return 0;
}



/* Read cycle and nanosec */
void
vt_read_ts(uint64_t *cy, uint64_t *ns) {

    _vt_read_cy(*cy);
    _vt_read_ns(*ns);

    return;

}


/* Get and record an event reading */
void
vt_read(char *ctag, int clen, uint32_t etag1, uint32_t etag2, uint32_t etag3) {
    uint64_t cy = 0, ns = 0, pmu1 = 0, pmu2 = 0, pmu3 = 0;

    vt_read_ts(&cy, &ns);
    //vt_read_pmu(etag, &pmu);
#ifdef __aarch64__
    sprintf(vt_data[next_id], "%s,%llu,%llu,%s,%llu,%s,%llu,%s,%llu", 
        ctag, cy, ns, vt_etags_arm[etag1], pmu1, vt_etags_arm[etag2],
        pmu2, vt_etags_arm[etag3], pmu3);
#else
    sprintf(vt_data[next_id], "%s,%llu,%llu,%s,%llu,%s,%llu,%s,%llu", 
        ctag, cy, ns, vt_etags_x64[etag1], pmu1, vt_etags_x64[etag2],
        pmu2, vt_etags_x64[etag3], pmu3);
#endif
    next_id ++;
    /* Check buffer health after reading */
    if (next_id >= _MSG_BUF_N - 2) {
        vt_write();
    }
}


/* Record a self-defined event value. */
void
vt_record(char *ctag, int clen, char *etag, int elen, int vt_type, void *val) {

}

/* Write data to file. */
void
vt_write() {
#ifdef USE_MPI
#else
    int i;
    vt_nmsg += next_id;
    vt_log(fp_log, "[vt_write] Write %d records, %d in total.\n", next_id, vt_nmsg);
    for(i = 0; i < next_id; i ++) {
        fprintf(fp_data, "%s\n", vt_data[i]);
    }
    memset(vt_data, '\0', _MSG_BUF_N * _MSG_LEN * sizeof(char));
    next_id = 0;
#endif
}

/* Exiting varapi */
void
vt_finalize() {
    vt_read("VT_END", 6, 0, 0, 0);
    vt_write();
    vt_putstamp(hostpath, timestr);
    vt_log(fp_log, "[vt_end] Varapi is finished.\n");
    fclose(fp_log);
    fclose(fp_data);
}