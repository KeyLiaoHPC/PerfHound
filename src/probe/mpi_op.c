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
#include "ph_core.h"

extern int ph_io_mkhost();
extern int ph_io_wtrankmap();

int
ph_mpi_rank_init() {
    int np, in;
    char hname[_HOST_MAX];
    if (MPI_Comm_size(MPI_COMM_WORLD, &np)) {
        return 1;
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &in);
    ph_pinfo.rank = in;
    ph_pinfo.nrank = np;
    ph_pinfo.cpu = sched_getcpu();
    ph_pinfo.head = 0;
    ph_pinfo.iorank = ph_pinfo.rank;
    gethostname(hname, _HOST_MAX);
    memcpy(ph_pinfo.host, hname, _HOST_MAX*sizeof(char));

    return 0;
}

void
ph_mpi_barrier(MPI_Comm comm) {
    MPI_Barrier(comm);

    return;
}

int
ph_mpi_mkhost() {
    int flag = 0, ioerr;
    int myrank, nrank;
    myrank = ph_pinfo.rank;
    nrank = ph_pinfo.nrank;
    MPI_Status stat;

    if (myrank == 0) {
        ioerr = ph_io_mkhost();
        if (ioerr) {
            return ioerr;
        }
        ioerr = ph_io_wtrankmap();
        if (ioerr) {
            return ioerr;
        }
        MPI_Send(&flag, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Recv(&flag, 1, MPI_INT, nrank-1, nrank-1, MPI_COMM_WORLD, &stat);
    } else {
        MPI_Recv(&flag, 1, MPI_INT, myrank-1, myrank-1, MPI_COMM_WORLD, &stat);
        ioerr = ph_io_mkhost();
        if (ioerr) {
            return ioerr;
        }
        ioerr = ph_io_wtrankmap();
        if (ioerr) {
            return ioerr;
        }
        MPI_Send(&flag, 1, MPI_INT, (myrank+1)%nrank, myrank, MPI_COMM_WORLD);
    }

    return 0;
}

