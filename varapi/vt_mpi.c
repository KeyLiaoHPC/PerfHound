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
#define _GNU_SOURCE

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>
#include <math.h>
#include <mpi.h>
#include "varapi_core.h"

/* Vars for IO group communicator. */
MPI_Comm comm_iogrp;
uint32_t mpi_size, iogrp_size, iogrp_rank, head, my_grank;
uint32_t *iogrp_grank, *iogrp_gcpu;              // IO group world rank.
MPI_Datatype vt_mpidata_t;  // MPI datatype for event data.
int64_t bns, wait_ns;                // bns = tx - t0, dnsclock bias to rank 0;


void
vt_get_rank(uint32_t *nrank, uint32_t *myrank) {
    /* Init */
    // CAUTION: Here we are not resposible for checking if the main program has been
    // successfully loaded into MPI environment.
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_grank);

    *nrank = mpi_size;
    *myrank = my_grank;

    return;
}

/* Sync and init Varapi MPI map info. */
int 
vt_sync_mpi_info(char *projpath, int run_id, uint32_t *head, int *iorank, 
                 uint32_t *vt_iogrp_size, uint32_t *vt_iogrp_grank, uint32_t *vt_iogrp_gcpu) {
    int i, j;
    int *cpu_all = NULL, mycpu;
    char *hname_all = NULL, myhost[_HOST_MAX];
    int mapped;
    uint32_t *head_all = NULL, *iorank_all = NULL;
    uint32_t head_now, head_next, iorank_cur;
    char cur_hname[_HOST_MAX];

    //printf("Start init. rank %u host %s cpu %u\n", myrank, myhost, mycpu);
    //fflush(stdout);
    //MPI_Barrier(MPI_COMM_WORLD);
    /* Gathering hostnames from all mpi ranks. */
    if (my_grank == 0) {
        hname_all = (char *)malloc(mpi_size * _HOST_MAX * sizeof(char));
        cpu_all = (int *)malloc(mpi_size * sizeof(int));
    }
    // Get hostnames.
    gethostname(myhost, _HOST_MAX);
    mycpu = sched_getcpu();
    MPI_Gather(myhost, _HOST_MAX, MPI_CHAR, hname_all, _HOST_MAX, MPI_CHAR, 0, MPI_COMM_WORLD);
    // Get cpu bindings.
    MPI_Gather(&mycpu, 1, MPI_UINT32_T, cpu_all, 1, MPI_UINT32_T, 0, MPI_COMM_WORLD);

    /* Determine head and iorank */
    if (my_grank == 0) {
        head_all = (uint32_t *)malloc(mpi_size * sizeof(uint32_t));
        iorank_all = (uint32_t *)malloc(mpi_size * sizeof(uint32_t));
        for (i = 0; i < mpi_size; i ++) {
            head_all[i] = 0;
            iorank_all[i] = 0;
        }
        // Here we use rank+1 to replace real rank for now, will minus one after scan
        head_now = 0;
        head_next = head_now;
        iorank_cur = head_now;
        iogrp_size = 1;
        head_all[0] = head_now + 1;
        iorank_all[0] = head_now;
        i = head_now;
        memcpy(cur_hname, &hname_all[0], _HOST_MAX);
    }
    
    /* Scan rank list, map head rank in host and assign io rank to every process. */
    if (my_grank == 0) {
        i = 0;
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
                    if (iogrp_size >= _RANK_PER_IO) {
                        iogrp_size = 0;
                        iorank_cur = i;
                    }
                    iorank_all[i] = iorank_cur;
                    iogrp_size += 1;
                }
            } // END: if (head_all[i] == 0)
            i += 1;
            // One host finished
            if (i >= mpi_size) {
                // check if there's any host left
                if (head_now != head_next) {
                    memcpy(cur_hname, hname_all + head_next * _HOST_MAX, _HOST_MAX * sizeof(char));
                    head_now = head_next;
                    i = head_now;
                    iorank_cur = i;
                    head_all[i] = head_now + 1;
                    iorank_all[i] = iorank_cur;
                    iogrp_size = 1;
                } else {
                    // all host has been set, quit loop.
                    break;
                }
            }
        } // END: while(1)

        for (i = 0; i < mpi_size; i ++) {
            head_all[i] -= 1;
        }
    } // END: if (my_grank == 0)

    
    MPI_Barrier(MPI_COMM_WORLD);

    /* Scatter host main rank and io rank to all ranks. */
    MPI_Scatter(head_all, 1, MPI_UINT32_T, head, 1, MPI_UINT32_T, 0, MPI_COMM_WORLD);
    MPI_Scatter(iorank_all, 1, MPI_UINT32_T, iorank, 1, MPI_UINT32_T, 0, MPI_COMM_WORLD);

    /* Create new communicator. */
    MPI_Comm_split(MPI_COMM_WORLD, *iorank, my_grank, &comm_iogrp);
    MPI_Comm_rank(comm_iogrp, &iogrp_rank);
    MPI_Comm_size(comm_iogrp, &iogrp_size);
    if (my_grank == *iorank) {
        printf("%d: %d\n", my_grank, iogrp_size);
        fflush(stdout);
    }

    iogrp_grank = (uint32_t *)malloc(iogrp_size * sizeof(uint32_t));
    iogrp_gcpu =  (uint32_t *)malloc(iogrp_size * sizeof(uint32_t));
    MPI_Gather(&my_grank, 1, MPI_UINT32_T, iogrp_grank, 1, MPI_UINT32_T, 0, comm_iogrp);
    MPI_Gather(&mycpu, 1, MPI_UINT32_T, iogrp_gcpu, 1, MPI_UINT32_T, 0, comm_iogrp);
    MPI_Bcast(iogrp_grank, iogrp_size, MPI_UINT32_T, 0, comm_iogrp);
    MPI_Bcast(iogrp_gcpu, iogrp_size, MPI_UINT32_T, 0, comm_iogrp);
    MPI_Barrier(MPI_COMM_WORLD);

    /* Gather and record group rank. */
    uint32_t *iogrp_rank_all;

    if (my_grank == 0) { 
        iogrp_rank_all = (uint32_t *)malloc(mpi_size * sizeof(uint32_t));
    }
    MPI_Gather(&iogrp_rank, 1, MPI_UINT32_T, iogrp_rank_all, 1, MPI_UINT32_T, 0, MPI_COMM_WORLD);

    /* Write to run?_rankmap.csv */
    if (my_grank == 0) {
        char map_file[_PATH_MAX];
        FILE *fp;

        sprintf(map_file, "%s/run%d_rankmap.csv", projpath, run_id);
        // TODO: Debug info
        fp = fopen(map_file, "w");
        fprintf(fp, "rank,hostname,cpu,hostmain,io_head,io_rank\n");
        for (i = 0; i < mpi_size; i ++) {
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

    *vt_iogrp_size = iogrp_size;
    for (i = 0; i < iogrp_size; i ++) {
        vt_iogrp_grank[i] = iogrp_grank[i];
        vt_iogrp_gcpu[i] = iogrp_gcpu[i];
        if (my_grank == *iorank) {
            //printf("%d, %d: %d %d\n", i, my_grank, iogrp_gcpu[0], vt_iogrp_gcpu[0]);
            fflush(stdout);
        }
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

/* Get io group cpu and global rank. */
void
vt_iogrp_getinfo(uint32_t *nrank, uint32_t *vt_iogrp_grank, uint32_t *vt_iogrp_gcpu) {
    int i = 0;

    *nrank = iogrp_size;
    for (i = 0; i < iogrp_size; i ++) {
        vt_iogrp_grank[i] = iogrp_grank[i];
        vt_iogrp_gcpu[i] = iogrp_gcpu[i];
    }
}

/* MPI type for data. */
void 
vt_newtype() {
    int nb, err;
    data_t vtdata;
#ifdef _N_EV
#ifdef _N_UEV
    int len[6];
    MPI_Aint disp[6];
    MPI_Datatype types[6];
    nb = 6;
#else
    int len[5];
    MPI_Aint disp[5];
    MPI_Datatype types[5];
    nb = 5;
#endif // END: #ifdef _N_UEV
#else
    int len[4];
    MPI_Aint disp[4];
    MPI_Datatype types[4];
    nb = 3;
#endif // END: #ifdef _N_EV

    types[0] = MPI_UINT32_T;
    types[1] = MPI_UINT64_T;
    types[2] = MPI_UINT64_T;
    types[3] = MPI_DOUBLE;
    len[0] = 2;
    len[1] = 1;
    len[2] = 1;
    len[3] = 1;
    disp[0] = (uint64_t)vtdata.ctag -(uint64_t)&vtdata;
    disp[1] = (uint64_t)&vtdata.cy - (uint64_t)&vtdata;
    disp[2] = (uint64_t)&vtdata.ns - (uint64_t)&vtdata;
    disp[3] = (uint64_t)&vtdata.uval - (uint64_t)&vtdata;

#ifdef _N_EV
    types[4] = MPI_UINT64_T;
    len[4] = _N_EV;
    disp[4] = (uint64_t)vtdata.ev - (uint64_t)&vtdata.ns;
#ifdef _N_UEV
    types[5] = MPI_DOUBLE;
    len[5] = _N_UEV;
    disp[5] = (uint64_t)vtdata.uev - (uint64_t)vtdata.ev;
#endif

#endif

    err = MPI_Type_create_struct(nb, len, disp, types, &vt_mpidata_t);
    err = MPI_Type_commit(&vt_mpidata_t);
    
    MPI_Barrier(MPI_COMM_WORLD);
    return;
}

/* MPI type destroy */
void 
vt_freetype() {
    MPI_Type_free(&vt_mpidata_t);

    return;
}

/* IO group barrier */
void
vt_io_barrier() {
    MPI_Barrier(comm_iogrp);

    return;
}

/* MPI_COMM_WORLD barrier */
void
vt_world_barrier() {
    MPI_Barrier(MPI_COMM_WORLD);

    return;
}

/* IO rank gets vartect data from a group member. */
int
vt_get_data(uint32_t rank, uint32_t count, data_t *data) {
    if (rank) {
        int err;
        MPI_Status st;

        err = MPI_Recv(data, count, vt_mpidata_t, rank, rank, comm_iogrp, &st);
        if (err) {
            return -1;
        }
    }

    return 0;
}

/* Send vartect data to io rank. */
void
vt_send_data(uint32_t count, data_t *data) {
    MPI_Send(data, count, vt_mpidata_t, 0, iogrp_rank, comm_iogrp);
}

/* Release mpi resource but don't touch main programm ! */
void 
vt_mpi_clean() {
    MPI_Comm_free(&comm_iogrp);
    free(iogrp_grank);
    free(iogrp_gcpu);

    return;
}


/* Get core time bias in ns, ns_sync = ns_now + bns. */
void
vt_get_bias(int r0, int r1) {
    struct timespec ts; 
    uint64_t ns0, ns1, ns2;
    double d_bns;
    MPI_Status st;
    
/*
 *  Step    Rank 0      Rank 1
 *   1      get ns0     nop
 *   2      send ns0    nop
 *   3      nop         recv ns0
 *          // first sync
 *   4      nop         get ns0
 *   5      nop         send ns0
 *   6      get ns1     nop
 *   7      send ns1    nop
 *   8      nop         recv ns1
 *   9      nop         get ns2
 *   10     bns = ns1 - (ns2 - ns0) * 0.5 - ns0
 *
 */
    // warm icache
    _vt_read_ns(ns0);

    if (my_grank == r0) {
        _vt_read_ns(ns0);
        MPI_Send(&ns0, 1, MPI_UINT64_T, r1, 101, MPI_COMM_WORLD);
        // First sync
        MPI_Recv(&ns0, 1, MPI_UINT64_T, r1, 102, MPI_COMM_WORLD, &st);
        _vt_read_ns(ns0);
        MPI_Send(&ns0, 1, MPI_UINT64_T, r1, 103, MPI_COMM_WORLD);
        bns = 0;
    }

    if (my_grank == r1) {
        MPI_Recv(&ns0, 1, MPI_UINT64_T, r0, 101, MPI_COMM_WORLD, &st);
        // First sync
        _vt_read_ns(ns0);
        MPI_Send(&ns0, 1, MPI_UINT64_T, r0, 102, MPI_COMM_WORLD);
        MPI_Recv(&ns1, 1, MPI_UINT64_T, r0, 103, MPI_COMM_WORLD, &st);
        _vt_read_ns(ns2);

        d_bns = (double)ns1 - (double)(ns2 - ns0) * 0.5 - (double)ns0;
        bns = (int64_t)d_bns;
    }
    MPI_Barrier(MPI_COMM_WORLD);

    return;
}

/* sync all ranks */
void
vt_tsync() {
    uint64_t ns, ns_new;
    int64_t tmp;
    struct timespec ts;

    MPI_Barrier(MPI_COMM_WORLD);
    _vt_read_ns(ns);
    MPI_Bcast(&ns, 1, MPI_UINT64_T, 0, MPI_COMM_WORLD);
    
    ns = ns + _SYNC_NS_OFFSET + bns;
    //ns = bns < 0? (ns - abs(bns)) : (ns + bns);

    _vt_read_ns(ns_new);
    while (ns_new < ns) {
        _vt_read_ns(ns_new);
    }

    return;
}

/* Get execution offsets to rank 0. */
void
vt_get_alt() {
    uint64_t ns, ns_r0;
    struct timespec ts;

    _vt_read_ns(ns);
    ns_r0 = ns;
    MPI_Bcast(&ns_r0, 1, MPI_UINT64_T, 0, MPI_COMM_WORLD);
    // wait_ns < 0: This rank is running slower (reach the sync point later) than rank 0,
    // wait_ns > 0: This rank is running faster (reach the sync point earlier) than rank 0.
    wait_ns = (int64_t) (ns_r0 - bns) - ns; 

}

/* Restore the variation according to alt bias. */
void
vt_atsync() {
    uint64_t ns, ns_new;
    int64_t wait_ns_min;
    int64_t register tmp;
    struct timespec ts;

    // How much slower from the slowest rank ? Get the minimum wait_ns;
    MPI_Allreduce(&wait_ns, &wait_ns_min, 1, MPI_INT64_T, MPI_MIN, MPI_COMM_WORLD);
    // Current rank 0 reading
    _vt_read_ns(ns);
    MPI_Bcast(&ns, 1, MPI_UINT64_T, 0, MPI_COMM_WORLD);
    // offset needs to contain the minimum wait_ns
    tmp = bns + _SYNC_NS_OFFSET - wait_ns_min - wait_ns;
    ns = ns + (uint64_t)tmp;
    // Reset the unbalance

    _vt_read_ns(ns_new);
    while (ns_new < ns) {
        _vt_read_ns(ns_new);
    }

    return;
}
