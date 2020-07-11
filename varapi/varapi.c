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

/* Variables for Varapi's control */
int vt_run_id;

/* Vartect data */
data_t *pdata;                          // Varapi raw data.
uint32_t vt_nmsg, vt_databuf;            // # of recorded messsages and buf capacity
uint32_t next_id;                       // Next data id.
#ifndef NETAG0
uint32_t etags[_N_ETAG];
#endif

/* Variables for files and information */
//char vt_data[_MSG_BUF_N][_MSG_LEN];
char hostpath[_PATH_MAX];                       // Data path for the host.
char projpath[_PATH_MAX];                       // Data path for the host.
char cfile[_PATH_MAX], efile[_PATH_MAX];        // Full path of ctag and etag.
char logfile[_PATH_MAX], datafile[_PATH_MAX];   // Full path of log and data.
// This timestamp is only for project records.
char timestr[16];                               // Timestamp records. One stamp per line.
FILE *fp_log;
#ifdef USE_MPI
FILE **fp_data;
#else
FILE *fp_data;
#endif

/* MPI and process info*/
uint32_t vt_nrank, vt_myrank, vt_mycpu;             // # of mpi ranks, rank id, core id.
uint32_t vt_iorank, vt_head;                        // My IO rank and host head rank.
char vt_myhost[_HOST_MAX];                          // My host name
#ifdef USE_MPI
uint32_t vt_iogrp_nrank, *vt_iogrp_grank, *vt_iogrp_gcpu;
#endif

/* Var for nanosec reading */
#if defined(__aarch64__) && defined(USE_CNTVCT)
uint64_t nspt;  // nanosecond per tick, only for aarch64. 
#else
struct timespec ts;
#endif


/* Initializing varapi */
int
vt_init(char *u_data_root, char *u_proj_name, uint32_t *u_etags) {
    // User-defined data root and project name.
    char udroot[_PATH_MAX], upname[_PATH_MAX];
    int vterr = 0;

#ifdef USE_MPI
    // Get process information.
    uint32_t vt_iogrp_grank[_RANK_PER_IO];
    uint32_t vt_iogrp_gcpu[_RANK_PER_IO];
    vt_get_rank(&vt_nrank, &vt_myrank);
    /* Notice user for vartect environment */
    if (vt_myrank == 0) {
        printf("*** [VarAPI] You are running in Vartect environment. "
               "Your code has been compiled with VarAPI-MPI. ***\n");
    }
#else
    vt_nrank = 1;
    vt_myrank = 0;
    printf("*** [VarAPI] You are running in Vartect environment. "
            "Your code has been compiled with VarAPI-Serial. ***\n");
#endif

    /* Get host and process info */
    gethostname(vt_myhost, _HOST_MAX);
    vt_mycpu = sched_getcpu();

    /* Gnerate path. */
    if (u_data_root == NULL) {
        sprintf(udroot, "./vt_data");
    } else {
        // TODO: for now, no path syntax check here.
        sprintf(udroot, "%s", u_data_root);
    }
    if (u_proj_name == NULL) {
        sprintf(upname, "vt_test");
    } else {
        // TODO: for now no syntax check here.
        sprintf(upname, u_proj_name);
    }
    // Path for project directory and host drectory which can be relative here.
    sprintf(projpath, "%s/%s", udroot, upname);
    sprintf(hostpath, "%s/%s", projpath, vt_myhost);

    /* Create project directory and init process info */
    if (vt_myrank == 0) {
        vt_mkdir(projpath);
    }
    if (vterr) {
        printf("*** [VarAPI] EXIT. ERROR: Fail before initializing VarAPI MPI. [Rank %d] ***\n", vt_myrank);
        exit(1);
    }
#ifdef USE_MPI
    // Get process information.
    vterr = vt_sync_mpi_info(vt_myhost, vt_nrank, vt_myrank, vt_mycpu, 
                             &vt_head, &vt_iorank, projpath, vt_run_id, 
                             &vt_iogrp_nrank, vt_iogrp_grank, vt_iogrp_gcpu);

#else
    vt_head = 0;
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
    if (vt_myrank == vt_head) {
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
        vt_bcast_tstamp(timestr);
#endif
    if (vt_myrank == vt_head) {
        sprintf(logfile, "%s/varapi_run%d_%s_%s.log", hostpath, vt_run_id, vt_myhost, timestr);
        // Initialize logging, get current run id in the project
        fp_log = NULL;
        fp_log = fopen(logfile, "w");
        if(fp_log == NULL) {
            printf("*** [VarAPI] EXIT. ERROR: VARAPI log file init failed. ***\n");
            exit(1);
        }
        vt_log(fp_log, "[vt_init] VarAPI started.\n");
    }   // END: if(vt_myrank == vt_iorank)

    /* IO rank creates data files. */
#ifdef USE_MPI
    //printf("%d: %d: %d: %s\n", vt_myrank, vt_iorank, vt_iogrp_nrank, datafile);
    if (vt_myrank == vt_iorank) {
        int i;
        // Create data files.
        fp_data = (FILE **)malloc(vt_iogrp_nrank * sizeof(FILE *));
        for (i = 0; i < vt_iogrp_nrank; i ++) {
            sprintf(datafile, "%s/run%d_r%d_c%d_all.csv", 
                    hostpath, vt_run_id, vt_iogrp_grank[i], vt_iogrp_gcpu[i]);
            fp_data[i] = fopen(datafile, "w");
            // TODO: Force quit.
            if (fp_data[i] == NULL) {
                printf("Failed creating datafile! Rank %d\n", vt_iogrp_grank[i]);
                exit(1);
            }
        }
    }
    vt_world_barrier();
#else
    sprintf(datafile, "%s/run%d_r%d_c%d_all.csv", hostpath, vt_run_id, vt_myrank, vt_mycpu);
    fp_data = NULL;
    fp_data = fopen(datafile, "w");
#endif


    /* Init data space. */
#ifdef USE_MPI
    vt_newtype();
#endif 
    vt_databuf = _MSG_BUF_KIB * 1024 / sizeof(data_t);
    pdata = (data_t *)malloc(vt_databuf * sizeof(data_t));
    vt_nmsg = 0;
    if (vt_myrank == vt_head) {
        vt_log(fp_log, "[vt_init] Message buffer: %u pieces, %.2f KiB\n", vt_databuf, 
               (double)vt_databuf * sizeof(data_t) / 1024);
    } 

    /* Init wall clock timer. Implenmetations vary with predefined macros. */
    _vt_init_ns;

    /* Initial time reading. */
    next_id = 0;
    vt_read("VT_START", 8);
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
vt_read(char *ctag, int clen) {
    uint64_t cy = 0, ns = 0;

    vt_read_ts(&cy, &ns);
    //vt_read_pmu(etag, &pmu);
    pdata[next_id].cy = cy;
    pdata[next_id].ns = ns;
    memcpy(pdata[next_id].ctag, ctag, clen);
    pdata[next_id].ctag[clen] = '\0';
#ifndef NETAG0
    int i;
    for (i = 0; i < _N_ETAG; i ++) {
        //vt_read_pmu(etags[i], &(pdata[next_id].pmu[i]));
        // TODO: no pmu yet.
        pdata[next_id].pmu[i] = 0;
    }
#endif

    next_id ++;
    /* Check buffer health after reading */
    if (next_id >= vt_databuf) {
        vt_write();
        next_id = 0;
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
    int i, j, k;
    
    vt_io_barrier();
    if (vt_myrank == vt_iorank) {
        for (i = 0; i < vt_iogrp_nrank; i ++) {
            vt_get_data(i, next_id, pdata);
            for(j = 0; j < next_id; j ++) {
#ifdef NETAG0
                fprintf(fp_data[i], "%s,%llu,%llu\n", pdata[j].ctag, pdata[j].cy, pdata[j].ns);
#else
                fprintf(fp_data[i], "%s,%llu,%llu,", pdata[j].ctag, pdata[j].cy, pdata[j].ns);
                for (k = 0; k < _N_ETAG - 1; k ++) {
                    fprintf(fp_data[i], "%llu,", pdata[j].pmu[k]);
                }
                fprintf(fp_data[i], "%llu\n", pdata[i].pmu[_N_ETAG-1]);
#endif
            }
        }
    } else {
        vt_send_data(next_id, pdata);
    } // END: if (vt_myrank == vt_iorank)
#else
    int i, j;

    for(i = 0; i < next_id; i ++) {
#ifdef NETAG0
        fprintf(fp_data, "%s,%llu,%llu\n", pdata[i].ctag, pdata[i].cy, pdata[i].ns);
#else
        fprintf(fp_data, "%s,%llu,%llu,", pdata[i].ctag, pdata[i].cy, pdata[i].ns);
        for (j = 0; j < _N_ETAG - 1; j ++) {
            fprintf(fp_data, "%llu,", pdata[i].pmu[j]);
        }
        fprintf(fp_data, "%llu\n", pdata[i].pmu[j]);
#endif // END: #ifdef NETAG0
    }
#endif // END: #ifdef USE_MPI

    vt_nmsg += next_id;
    if (vt_myrank == vt_head) {
        vt_log(fp_log, "[vt_write] Write %d records, %d in total.\n", next_id, vt_nmsg);
    }
    next_id = 0;

    return;
}

/* Exiting varapi */
void
vt_clean() {
    int i = 0;

    vt_read("VT_END", 6);
    vt_write();
    if (vt_myrank == vt_head) {
        vt_putstamp(hostpath, timestr);
        vt_log(fp_log, "[vt_end] Varapi done.\n");
    }
#ifdef USE_MPI
    if (vt_myrank == vt_iorank) {
        for (i = 0; i < vt_iogrp_nrank; i ++) {
            fclose(fp_data[i]);
        }
        free(fp_data);
    }
    if (vt_myrank == 0) {
        printf("*** [VarAPI] VarAPI Exited. ***\n");
        fclose(fp_log);
    }
#else
    printf("*** [VarAPI] VarAPI Exited. ***\n");
    fclose(fp_log);
    fclose(fp_data);
#endif
    free(pdata);
#ifdef USE_MPI
    vt_mpi_clean();
#endif
}