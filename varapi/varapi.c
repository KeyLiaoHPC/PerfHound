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
#include <errno.h>
#include "varapi.h"
#include "varapi_core.h"
#include "vt_etag.h"



/* Variables for Varapi's control */
int vt_run_id;

/* Vartect data */
data_t *pdata;                          // Varapi raw data.
uint32_t vt_nmsg, vt_databuf;            // # of recorded messsages and buf capacity
uint32_t vt_i;                       // Next data id.
int tag_commited;

#ifndef VT_MODE_TS
int vt_nev;
uint32_t vt_etags[_N_EV];
#endif

/* Variables for files and information */
char hostpath[_PATH_MAX];                       // Data path for the host.
char projpath[_PATH_MAX];                       // Data path for the host.
char vt_cfile[_PATH_MAX];                       // Full path of ctag.
char logfile[_PATH_MAX], datafile[_PATH_MAX];   // Full path of log and data.
// This timestamp is only for project records.
char timestr[16];                               // Timestamp records. One stamp per line
FILE *vt_flog;
#ifdef USE_MPI
FILE **vt_fdata;
#else
FILE *vt_fdata;
#endif

/* MPI and process info*/
uint32_t vt_nrank, vt_myrank, vt_mycpu;             // # of mpi ranks, rank id, core id.
uint32_t vt_iorank, vt_head;                        // My IO rank and host head rank.
char vt_myhost[_HOST_MAX];                          // My host name
#ifdef USE_MPI
uint32_t vt_iogrp_nrank, *vt_iogrp_grank, *vt_iogrp_gcpu;
#endif

/* Var for nanosec reading */
uint64_t nspt;  // nanosecond per tick, only for aarch64. 
struct timespec ts;

/* Initializing varapi */
int
vt_init(char *u_data_root, char *u_proj_name) {
    // User-defined data root and project name.
    char udroot[_PATH_MAX], upname[_PATH_MAX];
    int i, vterr = 0;

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
        vterr = vt_getstamp(projpath, timestr, &vt_run_id);
        if (vterr) {
            printf("*** [VarAPI] EXIT. ERROR: Fail before initializing VarAPI MPI. [Rank %d] ***\n", vt_myrank);
            exit(1);
        }
    }
#ifdef USE_MPI
    vt_world_barrier();
    // Get and sync process information. Print process map in the root of project.
    // <projpath>/run<run_id>_rankmap.csv
    // 
    vterr = vt_sync_mpi_info(projpath, &vt_run_id, &vt_head, &vt_iorank,
                             &vt_iogrp_nrank, vt_iogrp_grank, vt_iogrp_gcpu);
    //if (vt_myrank == vt_iorank)
    //    printf("%d: %d\n", vt_myrank, vt_iogrp_gcpu[0]);

#else
    vt_head = 0;
    vt_iorank = 0;

    FILE *fp_map;
    char rmap_path[_PATH_MAX];

    sprintf(rmap_path, "%s/run%d_rankmap.csv", projpath, vt_run_id);
    fp_map = fopen(rmap_path, "w");
    fprintf(fp_map, "rank,hostname,cpu,hostmain,io_head,io_rank\n");
    fprintf(fp_map, "%d,%s,%u,%u,%u,%u\n", 0, vt_myhost, vt_mycpu, 0, 0, 0);
    fclose(fp_map);
#endif

    /* Initialization of output directory and file*/
    /*
     * data path: <Data_Root>/<Proj_Name>/<Host_Name>/<files>
     */ 
    if (vt_myrank == vt_iorank) {
        vterr = vt_mkdir(hostpath);
    }

#ifdef USE_MPI
    vt_world_barrier();
#endif

    if (vterr) {
        printf("*** [VarAPI] EXIT. ERROR: VARAPI init failed. ***\n");
        exit(1);
    }

#ifdef USE_MPI
        vt_bcast_tstamp(timestr);
#endif
    if (vt_myrank == vt_head) {
        sprintf(logfile, "%s/varapi_run%d_%s_%s.log", hostpath, vt_run_id, vt_myhost, timestr);
        // Initialize logging, get current run id in the project
        vt_flog = NULL;
        vt_flog = fopen(logfile, "w");
        if(vt_flog == NULL) {
            printf("*** [VarAPI] EXIT. ERROR: VARAPI log file init failed. ***\n");
            exit(1);
        }
        vt_log(vt_flog, "[vt_init] VarAPI started.\n");
    }   // END: if(vt_myrank == vt_iorank)

    /* IO rank creates data files. */
#ifdef USE_MPI
    if (vt_myrank == vt_iorank) {
        // Create data files.
        vt_fdata = (FILE **)malloc(vt_iogrp_nrank * sizeof(FILE *));
        for (i = 0; i < vt_iogrp_nrank; i ++) {
            sprintf(datafile, "%s/run%d_r%d_c%d_all.csv", 
                    hostpath, vt_run_id, vt_iogrp_grank[i], vt_iogrp_gcpu[i]);
            //printf("%s\n", datafile);
            vt_fdata[i] = fopen(datafile, "w");
            if (vt_fdata[i] == NULL) {
                printf("Failed creating datafile! Rank %d, ERRNO %d\n", vt_iogrp_grank[i], errno);
                exit(1);
            }
            // Add header.
#ifdef  VT_MODE_SHORT
            fprintf(vt_fdata[i], "gid,pid,cycle,nanosec,uval,,ev1,ev2,ev3,ev4\n");

#elif   VT_MODE_LONG
            fprintf(vt_fdata[i], "gid,pid,cycle,nanosec,uval,,ev1,ev2,ev3,ev4,ev5,ev6,ev7,ev8,ev9,ev10,ev11,ev12\n");

#else
            fprintf(vt_fdata[i], "gid,pid,cycle,nanosec,uval,\n");

#endif // END: #ifdef  VT_MODE_SHORT
        }
    }
#else
    sprintf(datafile, "%s/run%d_r%d_c%d_all.csv", hostpath, vt_run_id, vt_myrank, vt_mycpu);
    vt_fdata = NULL;
    vt_fdata = fopen(datafile, "w");
    if (vt_fdata == NULL) {
        printf("Failed creating datafile! ERRNO %d\n", errno);
        exit(1);
    }

#ifdef  VT_MODE_SHORT
    fprintf(vt_fdata, "gid,pid,cycle,nanosec,uval,ev1,ev2,ev3,ev4\n");

#elif   VT_MODE_LONG
    fprintf(vt_fdata, "gid,pid,cycle,nanosec,uval,ev1,ev2,ev3,ev4,ev5,ev6,ev7,ev8,ev9,ev10,ev11,ev12\n");

#else
    fprintf(vt_fdata, "gid,pid,cycle,nanosec,uval\n");

#endif // END: #ifdef  VT_MODE_SHORT
#endif // END: #ifdef USE_MPI


    /* Init data space. */
#ifdef USE_MPI
    vt_world_barrier();
    vt_newtype();
#endif 
    vt_databuf = _MSG_BUF_KIB * 1024 / sizeof(data_t);
    pdata = (data_t *)malloc(vt_databuf * sizeof(data_t));
    vt_nmsg = 0;
    if (vt_myrank == vt_head) {
        vt_log(vt_flog, "[vt_init] Message buffer: %u pieces, %.2f KiB\n", vt_databuf, 
               (double)vt_databuf * sizeof(data_t) / 1024);
    } 

    /* Init wall clock timer. Implenmetations vary with predefined macros. */
    _vt_init_ts;

    /* Initial time reading. */
#ifdef USE_MPI
    vt_world_barrier();
    if (vt_myrank == vt_head) {
        vt_log(vt_flog, "[vt_init] Time sync started.\n");
    }
    for (i = 1; i < vt_nrank; i ++) {
        vt_get_bias(0, i);
    }
    
    if (vt_myrank == vt_head) {
        vt_log(vt_flog, "[vt_init] Time sync finished.\n");
    }
#endif
    if (vt_myrank == 0) {
        printf("*** [VarAPI] Init finished, start your main program. ***\n");
    }
    if (vt_myrank == vt_head) {
        vt_log(vt_flog, "[vt_init] Init finished.\n");
    }
#ifdef USE_MPI
    //vt_world_barrier();
    vt_tsync();
    
#endif
    vt_i = 0;
#ifndef VT_MODE_TS
    vt_nev = 0;
    vt_nuev = 0;
    tag_commited = 0;
#else
    tag_commited = 1;
#endif
    /* Create files for event tags and counting point tags. */
    if (vt_myrank == 0) {
        char fetag[_PATH_MAX];
        FILE *fp;

        sprintf(fetag, "%s/run%d_etags.csv", projpath, vt_run_id);
        fp = fopen(fetag, "a");
        fclose(fp);

        sprintf(vt_cfile, "%s/run%d_ctags.csv", projpath, vt_run_id);
        fp = fopen(vt_cfile, "w");
        fprintf(fp, "group_id,point_id,name\n");
        fclose(fp);
    }

#ifdef __aarch64__
    _vt_cy_init;
#endif
    vt_read(0, 1, 0, 0, 0);
    //vt_write();
    return 0;
}

/* Set system events. */
void
vt_set_ev(uint32_t *etag, int n) {
#ifdef _N_EV
    if (tag_commited) return;

    int i;

    vt_nev = n;
    if (n > _N_EV) {
        vt_nev = _N_EV;
    }
    for (i = 0; i < vt_nev; i ++) {
        vt_etags[i] = etag[i];
    }
#endif
}

/* Set group tag for counting points */
int 
vt_newgrp(uint32_t grp_id, const char *grp_name) {
    if (vt_myrank == 0) {
        FILE *fp;

        fp = fopen(vt_cfile, "a");
        fprintf(fp, "%u,0,%s\n", grp_id, grp_name);
        fclose(fp);
        fflush(fp);
    }
#ifdef USE_MPI
    vt_world_barrier();
#endif
    return 0;
}


/* Set counting points' tag. */
int 
vt_newtag(uint32_t grp_id, uint32_t p_id, const char *name) {
    if (vt_myrank == 0) {
        FILE *fp;

        fp = fopen(vt_cfile, "a");
        fprintf(fp, "%u,%u,%s\n", grp_id, p_id, name);
        fclose(fp);
        fflush(fp);
    }
    
#ifdef USE_MPI
    vt_world_barrier();
#endif

    return 0;
}

void
vt_commit() {
#ifdef _N_EV
    tag_commited = 1;
    if (vt_myrank == 0) {
        int i;
        char fetag[_PATH_MAX];
        FILE *fp;
        
        vt_log(vt_flog, "[vt_commit] User request %d system events, %d user-defined events.\n",
                vt_nev, vt_nuev);
        sprintf(fetag, "%s/run%d_etags.csv", projpath, vt_run_id);
        fp = fopen(fetag, "w");
        for (i = 0; i < vt_nev; i ++) {
#ifdef __x86_64__
            fprintf(fp, "x86_64,0x%x\n", vt_etags[i]);
#elif __aarch64__
            fprintf(fp, "aarch64,0x%x\n", vt_etags[i]);
#endif
        }
        fflush(fp);
        fclose(fp);
    }
#ifdef USE_MPI
    vt_world_barrier();
#endif 
#endif
}

/* Get and record an event reading */
void
vt_read(uint32_t grp_id, uint32_t p_id, double uval, int auto_write, int read_ev) {
    pdata[vt_i].ctag[0] = grp_id;
    pdata[vt_i].ctag[1] = p_id;
    _vt_read_cy (pdata[vt_i].cy);
    _vt_read_ns (pdata[vt_i].ns);
    pdata[vt_i].uval = uval;

    /* Read system event */
#ifdef _N_EV
    if (read_ev) {
        switch (vt_nev) {
            case 1:
                _vt_read_pm_1 (pdata[vt_i].ev);
                break;
            case 2:
                _vt_read_pm_2 (pdata[vt_i].ev);
                break;
            case 3:
                _vt_read_pm_3 (pdata[vt_i].ev);
                break;
            case 4:
                _vt_read_pm_4 (pdata[vt_i].ev);
                break;
            case 5:
                _vt_read_pm_5 (pdata[vt_i].ev);
                break;
            case 6:
                _vt_read_pm_6 (pdata[vt_i].ev);
                break;
            case 7:
                _vt_read_pm_7 (pdata[vt_i].ev);
                break;
            case 8:
                _vt_read_pm_8 (pdata[vt_i].ev);
                break;
            case 9:
                _vt_read_pm_9 (pdata[vt_i].ev);
                break;
            case 10:
                _vt_read_pm_10 (pdata[vt_i].ev);
                break;
            case 11:
                _vt_read_pm_11 (pdata[vt_i].ev);
                break;
            case 12:
                _vt_read_pm_12 (pdata[vt_i].ev);
                break;
            default:
                break;
        }
    } else {
        for (i = 0; i < _N_EV; i ++) {
            pdata[vt_i].ev[i] = 0;
        }
    }
#endif // END: #ifdef _N_EV

    vt_i ++;
    /* Check buffer health after reading */
    if (vt_i >= vt_databuf) {
        if (auto_write) {
            vt_write();
        } else {
            vt_i = 0;
        }
    }   
}


/* Write data to file. */
void
vt_write() {
    if (vt_i == 0) {
        return;
    }
    vt_read(0, 3, 0, 0, 0);

#ifdef USE_MPI
    /* MPI Write */
    int i, j, k;
    vt_get_alt();
    //vt_io_barrier();
    if (vt_myrank == vt_iorank) {
        for (i = 0; i < vt_iogrp_nrank; i ++) {
            vt_get_data(i, vt_i, pdata);
            for (j = 0; j < vt_i; j ++) {
                fprintf(vt_fdata[i], "%u,%u,%llu,%llu,%.15f", 
                        pdata[j].ctag[0], pdata[j].ctag[1], 
                        pdata[j].cy, pdata[j].ns, pdata[j].uval);
#ifdef  _N_EV
                for (k = 0; k < _N_EV; k ++) {
                    fprintf(vt_fdata[i], ",%llu", pdata[j].ev[k]);
                }
#endif
                fprintf(vt_fdata[i], "\n");
                fflush(vt_fdata[i]);
            }
        }
    } else {
        vt_send_data(vt_i, pdata);
    } // END: if (vt_myrank == vt_iorank)
#else
    /* No-MPI Write */
    int i, j;

    for(i = 0; i < vt_i; i ++) {
        fprintf(vt_fdata, "%u,%u,%llu,%llu,%.15f", 
                pdata[i].ctag[0], pdata[i].ctag[1], 
                pdata[i].cy, pdata[i].ns, pdata[i].uval);

#ifdef _N_EV
        for (k = 0; k < _N_EV; k ++) {
            fprintf(vt_fdata, ",%llu", pdata[j].ev[k]);
        }
#endif

        fprintf(vt_fdata, "\n");
        fflush(vt_fdata);
    }
#endif // END: #ifdef USE_MPI

    vt_nmsg += vt_i;
    if (vt_myrank == vt_head) {
        vt_log(vt_flog, "[vt_write] Write %d records, %d in total.\n", vt_i, vt_nmsg);
    }
    vt_i = 0;
#ifdef USE_MPI
    //vt_world_barrier();
    vt_atsync();
#endif
    vt_read(0, 4, 0, 0, 0);
    return;
}

/* Exiting varapi */
void
vt_clean() {
    int i = 0;

    vt_read(0, 2, 0, 1, 0);
    vt_write();
    if (vt_myrank == 0) {
        vt_putstamp(projpath, timestr);
    }
    if (vt_myrank == vt_head) {
        vt_log(vt_flog, "[vt_end] Varapi done.\n");
    }
#ifdef USE_MPI
    if (vt_myrank == vt_iorank) {
        for (i = 0; i < vt_iogrp_nrank; i ++) {
            fclose(vt_fdata[i]);
        }
        free(vt_fdata);
    }
    if (vt_myrank == 0) {
        printf("*** [VarAPI] VarAPI Exited. ***\n");
        fclose(vt_flog);
    }
#else
    printf("*** [VarAPI] VarAPI Exited. ***\n");
    fclose(vt_flog);
    fclose(vt_fdata);
#endif
    free(pdata);
    _vt_fini_ts;
#ifdef USE_MPI
    vt_mpi_clean();
#endif
}
