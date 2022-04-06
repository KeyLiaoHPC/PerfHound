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
 * vt_mpi.c
 * Description: Wrap Varapi mpi operations.
 * Author: Key Liao
 * Modified: May. 28th, 2020
 * Email: keyliaohpc@gmail.com
 * =================================================================================
 */

#define _GNU_SOURCE

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>
#include <math.h>
#include <mpi.h>
#include "pfh_core.h"

extern int pfh_io_mkhost();
extern int pfh_io_wtrankmap();

int
pfh_mpi_rank_init() {
    int np, in;
    char hname[_HOST_MAX];
    if (MPI_Comm_size(MPI_COMM_WORLD, &np)) {
        return 1;
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &in);
    pfh_pinfo.rank = in;
    pfh_pinfo.nrank = np;
    pfh_pinfo.cpu = sched_getcpu();
    pfh_pinfo.head = 0;
    pfh_pinfo.iorank = pfh_pinfo.rank;
    gethostname(hname, _HOST_MAX);
    memcpy(pfh_pinfo.host, hname, _HOST_MAX*sizeof(char));

    return 0;
}

void
pfh_mpi_barrier(MPI_Comm comm) {
    MPI_Barrier(comm);

    return;
}

int
pfh_mpi_mkhost() {
    int flag = 0, ioerr;
    int myrank, nrank;
    myrank = pfh_pinfo.rank;
    nrank = pfh_pinfo.nrank;
    MPI_Status stat;

    if (myrank == 0) {
        ioerr = pfh_io_mkhost();
        if (ioerr) {
            return ioerr;
        }
        ioerr = pfh_io_wtrankmap();
        if (ioerr) {
            return ioerr;
        }
        MPI_Send(&flag, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Recv(&flag, 1, MPI_INT, nrank-1, nrank-1, MPI_COMM_WORLD, &stat);
    } else {
        MPI_Recv(&flag, 1, MPI_INT, myrank-1, myrank-1, MPI_COMM_WORLD, &stat);
        ioerr = pfh_io_mkhost();
        if (ioerr) {
            return ioerr;
        }
        ioerr = pfh_io_wtrankmap();
        if (ioerr) {
            return ioerr;
        }
        MPI_Send(&flag, 1, MPI_INT, (myrank+1)%nrank, myrank, MPI_COMM_WORLD);
    }

    return 0;
}
