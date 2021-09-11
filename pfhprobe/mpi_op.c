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
#include "pfhprobe_core.h"

extern int pfh_io_mkhost();
extern int pfh_io_wtrankmap();

int
pfh_mpi_rank_init(proc_t *pinfo) {
    int np, in;
    char hname[_HOST_MAX];
    if (MPI_Comm_size(MPI_COMM_WORLD, &np)) {
        return 1;
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &in);
    pinfo->rank = in;
    pinfo->nrank = np;
    pinfo->cpu = sched_getcpu();
    pinfo->head = 0;
    pinfo->iorank = pfh_pinfo.rank;
    gethostname(hname, _HOST_MAX);
    memcpy(pinfo->host, hname, _HOST_MAX*sizeof(char));

    return 0;
}

void
pfh_mpi_barrier(MPI_Comm comm) {
    MPI_Barrier(comm);

    return;
}

int
pfh_mpi_mkhost(proc_t *pinfo) {
    int flag = 0, ioerr;
    MPI_Status stat;

    if (pinfo.rank == 0) {
        ioerr = pfh_io_mkhost();
        if (ioerr) {
            return ioerr;
        }
        MPI_Recv(&flag, 1, MPI_INT, pinfo->nrank-1, pinfo->nrank-1,
            MPI_COMM_WORLD, &stat);
    } else {
        MPI_Recv(&flag, 1, MPI_INT, pinfo->rank-1, pinfo->rank-1, 
            MPI_COMM_WORLD, &stat);
        ioerr = pfh_io_mkhost();
        if (ioerr) {
            return ioerr;
        }
        ioerr = pfh_io_wtrankmap(pinfo);
        if (ioerr) {
            return ioerr;
        }
        MPI_Send(&flag, 1, MPI_INT, (myrank+1)%nrank, myrank, MPI_COMM_WORLD);
    }
}

