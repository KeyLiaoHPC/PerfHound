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
 * vt_mpi.c
 * Description: Wrap Varapi mpi operations.
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

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include "varapi_core.h"

/* Vars for IO group communicator. */
MPI_Comm comm_iogrp;
uint32_t iogrp_size, iogrp_rank;
uint32_t *iogrp_grank;              // IO group world rank.

void
vt_get_rank(uint32_t *nrank, uint32_t *myrank) {
    /* Init */
    // CAUTION: Here we are not resposible for checking if the main program has been
    // successfully loaded into MPI environment.
    MPI_Comm_size(MPI_COMM_WORLD, nrank);
    MPI_Comm_rank(MPI_COMM_WORLD, myrank);

    return;
}

/* Sync and init Varapi MPI map info. */
int
vt_sync_mpi_info(char *myhost, uint32_t nrank, uint32_t myrank, uint32_t mycpu, 
                 uint32_t *head, int *iorank,
                 char *projpath, int run_id) {
    int i, j;
    int *cpu_all = NULL;
    char *hname_all = NULL;
    int mapped;
    uint32_t *head_all = NULL, *iorank_all = NULL;
    uint32_t head_now, head_next, iorank_cur, n_iorank;
    char cur_hname[_HOST_MAX];

    //printf("Start init. rank %u host %s cpu %u\n", myrank, myhost, mycpu);
    //fflush(stdout);
    //MPI_Barrier(MPI_COMM_WORLD);
    /* Gathering hostnames from all mpi ranks. */
    if (myrank == 0) {
        hname_all = (char *)malloc(nrank * _HOST_MAX * sizeof(char));
        cpu_all = (int *)malloc(nrank * sizeof(int));
    }
    // Get hostnames.
    MPI_Gather(myhost, _HOST_MAX, MPI_CHAR, hname_all, _HOST_MAX, MPI_CHAR, 0, MPI_COMM_WORLD);
    // Get cpu bindings.
    MPI_Gather(&mycpu, 1, MPI_UINT32_T, cpu_all, 1, MPI_UINT32_T, 0, MPI_COMM_WORLD);

    /* Determine head and iorank */
    if (myrank == 0) {
        head_all = (uint32_t *)malloc(nrank * sizeof(uint32_t));
        iorank_all = (uint32_t *)malloc(nrank * sizeof(uint32_t));
        for (i = 0; i < nrank; i ++) {
            head_all[i] = 0;
            iorank_all[i] = 0;
        }
        // Here we use rank+1 to replace real rank for now, will minus one after scan
        head_now = 0;
        head_next = head_now;
        iorank_cur = head_now;
        n_iorank = 2;
        head_all[0] = head_now + 1;
        iorank_all[0] = head_now;
        i = head_now;
        memcpy(cur_hname, &hname_all[0], _HOST_MAX);
    }
    
    /* Scan rank list, map head rank in host and assign io rank to every process. */
    if (myrank == 0) {
        while (1) {
            mapped = 1;
            // main host rank not set yet
            if (head_all[i] == 0) {
                // hostname comparison
                for (j = 0; j < _HOST_MAX; j ++) {
                    // not same with current main host
                    if (cur_hname[j] != hname_all[i*_HOST_MAX+j]) {
                        mapped = 0;
                        // update next host main rank id
                        if (head_now == head_next) {
                            head_next = i;
                        }
                        break;
                    }
                    // string end
                    if (cur_hname[j] == '\0' && hname_all[i*_HOST_MAX+j] == '\0') {
                        break;
                    }
                }
                if (mapped) {
                    head_all[i] = head_now + 1;
                    // One io rank works for _RANK_PER_IO ranks.
                    if (n_iorank > _RANK_PER_IO) {
                        n_iorank = 1;
                        iorank_cur = i;
                    }
                    iorank_all[i] = iorank_cur;
                    n_iorank += 1;
                }
            } // END: if (head_all[i] == 0)
            i += 1;
            // One host finished
            if (i >= nrank) {
                // check if there's any host left
                if (head_now != head_next) {
                    memcpy(cur_hname, hname_all + head_next * _HOST_MAX, _HOST_MAX * sizeof(char));
                    head_now = head_next;
                    i = head_now;
                    iorank_cur = i;
                    n_iorank = 1;
                    head_all[i] = head_now + 1;
                    iorank_all[i] = iorank_cur;
                    n_iorank += 1;
                } else {
                    // all host has been set, quit loop.
                    break;
                }
            }
        } // END: while(1)

        for (i = 0; i < nrank; i ++) {
            head_all[i] -= 1;
        }
    } // END: if (myrank == 0)

    
    MPI_Barrier(MPI_COMM_WORLD);

    /* Scatter host main rank and io rank to all ranks. */
    MPI_Scatter(head_all, 1, MPI_UINT32_T, head, 1, MPI_UINT32_T, 0, MPI_COMM_WORLD);
    MPI_Scatter(iorank_all, 1, MPI_UINT32_T, iorank, 1, MPI_UINT32_T, 0, MPI_COMM_WORLD);

    /* Create new communicator. */
    MPI_Comm_split(MPI_COMM_WORLD, *iorank, myrank, &comm_iogrp);
    MPI_Comm_rank(comm_iogrp, &iogrp_rank);
    MPI_Comm_rank(comm_iogrp, &iogrp_size);

    iogrp_grank = (uint32_t *)malloc(iogrp_size * sizeof(uint32_t));
    MPI_Gather(&myrank, 1, MPI_UINT32_T, iogrp_grank, 1, MPI_UINT32_T, 0, comm_iogrp);
    MPI_Barrier(MPI_COMM_WORLD);

    /* Gather and record group rank. */
    uint32_t *iogrp_rank_all;

    if (myrank == 0) { 
        iogrp_rank_all = (uint32_t *)malloc(nrank * sizeof(uint32_t));
    }
    MPI_Gather(&iogrp_rank, 1, MPI_UINT32_T, iogrp_rank_all, 1, MPI_UINT32_T, 0, MPI_COMM_WORLD);

    /* Write to run?_rankmap.csv */
    if (myrank == 0) {
        char map_file[_PATH_MAX];
        FILE *fp;

        sprintf(map_file, "%s/run%d_rankmap.csv", projpath, run_id);
        // TODO: Debug info
        fp = fopen(map_file, "w");
        printf("Mapping: %s, address: %x\n", map_file, fp);
        fprintf(fp, "rank,hostname,cpu,hostmain,iohead,grp_rank\n");
        for (i = 0; i < nrank; i ++) {
            fprintf(fp, "%d,%s,%u,%u,%u,%u\n", 
                    i, hname_all + i * _HOST_MAX, cpu_all[i], head_all[i], 
                    iorank_all[i], iogrp_rank_all[i]);
        } 
        fclose(fp);

        free(iogrp_rank_all);
        free(cpu_all);
        free(hname_all);
        free(head_all);
        free(iorank_all);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    return 0;
}

/* Broadcast rank 0's timestamp */
void
vt_bcast_tstamp(char *timestr) {
    MPI_Bcast(timestr, 16, MPI_CHAR, 0, MPI_COMM_WORLD);
    return;
}

void 
vt_mpi_finalize() {
    MPI_Comm_free(&comm_iogrp);
    free(iogrp_grank);

    return;
}