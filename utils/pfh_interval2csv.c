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
 * pfh_interval2csv.c
 * Description: Wrap Varapi mpi operations.
 * Author: Key Liao
 * Modified: May. 28th, 2020
 * Email: keyliaohpc@gmail.com
 * Usage:
 * 
 * mpicc -Wall -o pfh_interval2csv.x ./pfh_interval2csv.c
 * mpirun -np 10 ./pfh_interval2csv.x /data/run_16 1,1 1,2
 * This instruction will read data in /data/run_16, 
 * calculate data(gid=1,pid=2) - data(gid=1,pid=1)
 * Data will be write to /data/run_16/diff_1-1_1-2/r*c*.csv and all.csv
 * =================================================================================
 */
//==================================================================================
#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <sys/stat.h>
#include <mpi.h>

typedef struct {
    int sgid, egid, spid, epid, nf;
    int *cpus, *ranks;
    char ipath[PATH_MAX];
    char opath[PATH_MAX];
    char **hosts;
} arg_t;

int init_comm(int *myid, int *np);
int init_rankmap(int argc, char **argv, int myid, int np, arg_t *arg);
int init_arg(int argc, char **argv, arg_t *arg);
int calc_interval(arg_t *arg);
void step_to(char *str, char ch, char **p);

int
init_comm(int *myid, int *np) {
    int err;
    err = MPI_Init(NULL, NULL);
    if (err) {
        return err;
    }
    err = MPI_Comm_rank(MPI_COMM_WORLD, myid);
    if (err) {
        return err;
    }
    err = MPI_Comm_size(MPI_COMM_WORLD, np);
    if (err) {
        return err;
    }

    return 0;
}

/* Reading rankmap and parsing filenames */
int
init_rankmap(int argc, char **argv, int myid, int np, arg_t *arg) {
    int fcount, mod;
    long fsize;
    int *allcpus, *allranks;
    char *rmapstr, *prmap, **allhosts;
    FILE *fp;
    MPI_Status mpistat;

    if (myid == 0) {
        char rankpath[PATH_MAX];
        sprintf(rankpath, "%s/%s", argv[1], "rankmap.csv");
        /* Reading rank map. */
        fp = fopen(rankpath, "r");
        if (fp == NULL) {
            return errno;
        }
        fseek(fp, 0, SEEK_END);
        fsize = ftell(fp);
        rewind(fp);
        rmapstr = (char *)malloc(fsize+1);
        fread(rmapstr, 1, fsize, fp);
        fclose(fp);
        rmapstr[fsize] = '\0';

        /* Count rank. */
        fcount = 0;
        prmap = rmapstr;
        while (prmap[0]) {
            if (prmap[0] == '\n') {
                fcount ++;
            }
            prmap ++;
        }
        fcount -= 1;
        printf("There are %d rank records.\n", fcount);
        allcpus = (int *)malloc(fcount * sizeof(int));
        allranks = (int *)malloc(fcount * sizeof(int));
        allhosts = (char **)malloc(fcount * sizeof(char*));
        allhosts[0] = (char *)malloc(fcount * HOST_NAME_MAX * sizeof(char));
        for (int i = 1; i < fcount; i ++) {
            allhosts[i] = allhosts[0] + i * HOST_NAME_MAX;
        }

        /* Parse rank records' file names. */
        int idf = 0;
        prmap = rmapstr;
        while (prmap[0]) {
            prmap ++;
            if (prmap[0] == '\n') {
                prmap += 1;
                if (prmap[0] == '\0') {
                    break;
                }
                int i = 1;
                while (prmap[i] != ',') {
                    i ++;
                }
                prmap[i] = '\0';
                allranks[idf] = strtol(prmap, NULL, 10);
                prmap = prmap + i + 1;
                i = 1;
                while (prmap[i] != ',') {
                    i ++;
                }
                prmap[i] = '\0';
                sprintf(allhosts[idf], "%s", prmap);
                prmap = prmap + i + 1;
                i = 1;
                while (prmap[i] != ',') {
                    i ++;
                }
                prmap[i] = '\0';
                allcpus[idf] = strtol(prmap, NULL, 10);
                prmap = prmap + i + 1;
                idf ++;
            }
        }

        if (np > 1) {
            MPI_Bcast(&fcount, 1, MPI_INT, 0, MPI_COMM_WORLD);
        }

        printf("Record files:\n");
        for (int i = 0; i < fcount; i ++) {
            printf("%s/r%dc%d.csv\t", allhosts[i], allranks[i], allcpus[i]);
        }
        printf("\n===================\n");
    } else{
        MPI_Bcast(&fcount, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }

    /* Spreading */
    mod = fcount % np;
    arg->nf = fcount / np;
    if (myid < mod) {
        arg->nf += 1;
    }

    // Allocating each rank
    arg->cpus = (int *)malloc(arg->nf * sizeof(int));
    arg->ranks = (int *)malloc(arg->nf * sizeof(int));
    arg->hosts = (char **)malloc(arg->nf * sizeof(char*));
    arg->hosts[0] = (char *)malloc(arg->nf * HOST_NAME_MAX * sizeof(char));
    for (int i = 1; i < arg->nf; i ++) {
        arg->hosts[i] = arg->hosts[0] + i * HOST_NAME_MAX;
    }
    // Run dir.
    memcpy(arg->ipath, argv[1], strlen(argv[1])*sizeof(char));

    if (myid == 0) {
        for (int i = 0; i < arg->nf; i ++) {
            arg->cpus[i] = allcpus[i];
            arg->ranks[i] = allranks[i];
            memcpy(arg->hosts[i], allhosts[i], strlen(allhosts[i])*sizeof(char));
        }
        int n = fcount / np + 1;
        for (int i = 1; i < mod; i ++) {
            MPI_Send(allcpus + i*n, n, MPI_INT, i, i, MPI_COMM_WORLD);
            MPI_Send(allranks + i*n, n, MPI_INT, i, i, MPI_COMM_WORLD);
            MPI_Send(allhosts[i*n], n * HOST_NAME_MAX, MPI_CHAR, i, i, MPI_COMM_WORLD);
        }
        n = fcount / np;
        for (int i = mod; i < np; i ++) {
            fflush(stdout);
            MPI_Send(allcpus+mod*(n+1)+(i-mod)*n, n, MPI_INT, i, i, MPI_COMM_WORLD);
            MPI_Send(allranks+mod*(n+1)+(i-mod)*n, n, MPI_INT, i, i, MPI_COMM_WORLD);
            MPI_Send(allhosts[mod*(n+1)+(i-mod)*n], n * HOST_NAME_MAX, MPI_CHAR, i, i, MPI_COMM_WORLD);
        }
        free(allcpus);
        free(allranks);
        free(allhosts[0]);
        free(allhosts);
    } else {
        MPI_Recv(arg->cpus, arg->nf, MPI_INT, 0, myid, MPI_COMM_WORLD, &mpistat);
        MPI_Recv(arg->ranks, arg->nf, MPI_INT, 0, myid, MPI_COMM_WORLD, &mpistat);
        MPI_Recv(arg->hosts[0], arg->nf * HOST_NAME_MAX, MPI_CHAR, 0, myid, MPI_COMM_WORLD, &mpistat);
        printf("Jobs for rank %d: %d \n", myid, arg->nf);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    
    return 0;
}

/* parsing given point. */
int
init_arg(int argc, char **argv, arg_t *arg) {
    char *parg;
    int ipos, myid, err = 0;



    sprintf(arg->ipath, "%s", argv[1]);

    arg->sgid = 0;
    arg->egid = 0;
    arg->spid = 0;
    arg->epid = 0;
    /* Start point. */
    parg = argv[2];
    ipos = 0;
    for (int i = 0; i < strlen(parg); i ++) {
        if (parg[i] == ',') {
            ipos = i;
            parg[i] = '\0';
            break;
        }
    }
    arg->sgid = strtol(parg, NULL, 10);
    arg->spid = strtol(parg + ipos + 1, NULL, 10);
    
    /* End point. */
    parg = argv[3];
    ipos = 0;
    for (int i = 0; i < strlen(parg); i ++) {
        if (parg[i] == ',') {
            ipos = i;
            parg[i] = '\0';
            break;
        }
    }
    arg->egid = strtol(parg, NULL, 10);
    arg->epid = strtol(parg + ipos + 1, NULL, 10);

    if (arg->sgid == 0 || arg->egid == 0 || 
        arg->spid == 0 || arg->epid == 0) {
        return -1;
    }

    /* Output Path */
    sprintf(arg->opath, "%s/diff_%d-%d_%d-%d", 
            argv[1], arg->sgid, arg->spid, arg->egid, arg->epid);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    if (myid == 0) {
        err = mkdir(arg->opath, S_IRWXU);
        if (err != 0 && errno != EEXIST) {
            return err;
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);

    return 0;
}

inline void
step_to(char *str, char ch, char **p) {
    int i = 0;
    while (1) {
        if (str[i] == ch || str[i] == '\0') {
            *p = str + i;
            break;
        }
        i ++;
    }
}

int
calc_interval(arg_t *arg) {
    long fsize;
    int ncol, gid, pid, nev, id, myid, np;
    int myjob, nextjob, flag = 0, scount;
    MPI_Status mpistat;
    uint64_t cy, ns;
    double uval;
    char fname[10240], fnameall[10240];
    FILE *fp, *fpall;
    char *recstr = NULL, *outstr = NULL, *pst = NULL, *pen = NULL, *pout = NULL;
    uint64_t evs[12];

    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    sprintf(fname, "%s/%s/r%dc%d.csv",
            arg->ipath, arg->hosts[0], arg->ranks[0], arg->cpus[0]);
    
    /* Read the first record for counting the # of column */
    fp = fopen(fname, "r");
    if (fp == NULL) {
        return errno;
    }
    fseek(fp, 0, SEEK_END);
    fsize = ftell(fp);
    rewind(fp);
    recstr = (char *)realloc(recstr, fsize+1);
    fread(recstr, 1, fsize, fp);
    fclose(fp);

    /* Number of columns */
    recstr[fsize] = '\0';
    pst = recstr;
    pen = pst;
    ncol = 1;
    while (*pen != '\n') {
        if (*pen == ',') {
            ncol ++;
        }
        pen ++;
    }
    nev = ncol - 5;

    /* Rank 0 writes headers to all.vsb */
    sprintf(fnameall, "%s/all.csv", arg->opath);
    if (myid == 0) {
        fpall = fopen(fnameall, "w");
        printf("All data will be gathered to %s\n", fnameall);
        if (fpall == NULL) {
            return errno;
        }
        fprintf(fpall, "id,host,rank,cpu,cycle,nanosec,uval");
        for (int j = 0; j < nev; j ++) {
            fprintf(fpall, ",ev%d", j + 1);
        }
        fprintf(fpall, "\n");
        fflush(fpall);
        fclose(fpall);
    }

    myjob = arg->nf;
    if (myid == 0) {
        MPI_Recv(&nextjob, 1, MPI_INT, myid + 1, myid, MPI_COMM_WORLD, &mpistat);
    } else if (myid +1 == np) {
        MPI_Send(&myjob, 1, MPI_INT, myid - 1, myid - 1, MPI_COMM_WORLD);
    } else {
        MPI_Recv(&nextjob, 1, MPI_INT, myid + 1, myid, MPI_COMM_WORLD, &mpistat);
        MPI_Send(&myjob, 1, MPI_INT, myid - 1, myid - 1, MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    /* Main loop for inverval calculation. */
    for (int i = 0; i < arg->nf; i ++) {
        /* Reading data. */
        sprintf(fname, "%s/%s/r%dc%d.csv", 
                arg->ipath, arg->hosts[i], arg->ranks[i], arg->cpus[i]);
        printf("Rank %d: Processing %s\n", myid, fname);
        fflush(stdout);
        fp = fopen(fname, "r");
        fseek(fp, 0, SEEK_END);
        fsize = ftell(fp);
        rewind(fp);
        recstr = (char *)realloc(recstr, fsize+1);
        outstr = (char *)realloc(outstr, fsize+1);
        pout = outstr;
        fread(recstr, 1, fsize, fp);
        recstr[fsize] = '\0';
        pst = recstr;
        pen = pst;
        fclose(fp);
        fp = NULL;

        /* File for output. */
        sprintf(fname, "%s/r%dc%d.csv", 
                arg->opath, arg->ranks[i], arg->cpus[i]);
        printf("Rank %d: Data will write to %s (%d/%d)\n", myid, fname, i+1, arg->nf);
        fp = fopen(fname, "w");
        if (fp == NULL) {
            return errno;
        }
        /* Writing headers. */
        fprintf(fp, "id,host,rank,cpu,cycle,nanosec,uval");
        for (int j = 0; j < nev; j ++) {
            fprintf(fp, ",ev%d", j + 1);
        }
        fprintf(fp, "\n");
        fflush(fp);
        step_to(recstr, '\n', &pen);
        pst = pen + 1;
        fflush(stdout);

        id = 0;
        /* Start calculating */
        while (*pst) {
            // Search for start line
            gid = strtol(pst+1, &pst, 10);
            pid = strtol(pst+1, &pst, 10);
            if (gid != arg->sgid || pid != arg->spid) {
                step_to(pst+5+2*nev, '\n', &pst);
                // pst += 1;
                continue;
            }
            
            // Extract start values.
            cy = strtoull(pst+1, &pst, 10);
            ns = strtoull(pst+1, &pst, 10);
            uval = strtod(pst+1, &pst);
            for (int j = 0; j < nev; j ++) {
                evs[j] = strtoull(pst+1, &pst, 10);
                // pst = pen + 1;
            }
            // Search for end line.
            gid = strtol(pst+1, &pst, 10);
            // pst = pen + 1;
            pid = strtol(pst+1, &pst, 10);
            // pst = pen + 1;
            if (gid != arg->egid || pid != arg->epid) {
                step_to(pst+5+2*nev, '\n', &pst);
                // pst += 1;
                continue;
            }

            cy = strtoull(pst+1, &pst, 10) - cy;
            // pst = pen + 1;
            ns = strtoull(pst+1, &pst, 10) - ns;
            // pst = pen + 1;
#ifdef RDTSCP
            ns = ns * 1e6 / 2494103;
#endif

            strtod(pst+1, &pst); // Skip this uval
            // pst = pen + 1;
            for (int j = 0; j < nev; j ++) {
                evs[j] = strtoull(pst+1, &pst, 10) - evs[j];
                // pst = pen + 1;
            }

            // Write out.
            //printf("Rank %d, %d,%s,%d,%d,%lu,%lu,%.15f", myid,
            //        id, arg->hosts[i], arg->ranks[i], arg->cpus[i], cy, ns, uval);
            //fflush(stdout);
            scount = sprintf(   pout, "%d,%s,%d,%d,%lu,%lu,%.15f", 
                                id, arg->hosts[i], arg->ranks[i], arg->cpus[i], cy, ns, uval);
            pout = pout + scount;
            for (int j = 0; j < nev; j ++) {
                scount = sprintf(pout, ",%lu", evs[j]);
                pout = pout + scount;
            }
            scount = sprintf(pout, "\n");
            pout = pout + scount;

            // Next couple
            // if (myid == 0) {
            //     printf("%d\n", id);
            //     fflush(stdout);
            // }
            id ++;
            // pst = pen + 1;
        }
        fprintf(fp, "%s", outstr);
        fflush(fp);
        fclose(fp);
        /* Write to all.csv */

        if (myid != 0) {
            MPI_Recv(&flag, 1, MPI_INT, myid - 1, myid, MPI_COMM_WORLD, &mpistat);
        }

        fpall = fopen(fnameall, "a");
        if (fpall == NULL) {
            return errno;
        }
        fprintf(fpall, "%s", outstr);
        fflush(fpall);
        fclose(fpall);
        if (myid + 1 != np && nextjob != 0) {
            MPI_Send(&flag, 1, MPI_INT, myid + 1, myid + 1, MPI_COMM_WORLD);
            nextjob --;
        }


    }

    free(recstr);
    free(outstr);
    return 0;
}

int
main(int argc, char **argv) {
    int myid, np, err;
    arg_t arg;

    err = init_comm(&myid, &np);
    if (err) {
        printf("Failed to init MPI. EXIT %d\n", err);
    }

    if (myid == 0) {
        if (argc < 4) {
            printf("Usage:\n");
            printf("\tpfh_interval2csv.x <run_data_dir> start_gid,startpid end_gid,end_pid.\n");
            printf("Example:\n");
            printf("\tpfh_interval2csv.x /home/data/run_4 1,1 1,2\n");
            MPI_Finalize();
            exit(-1);
        }
    }

    if (myid == 0) {
        printf("Start parsing rankmap.csv\n");
        fflush(stdout);
    }
    err = init_rankmap(argc, argv, myid, np, &arg);
    // printf("%x\n", arg.hosts[0]);
    if (err) {
        if (myid == 0) {
            printf("Failed to parse rankmap. EXIT %d.\n", err);
            exit(-1);
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if (myid == 0) {
        printf("Start parsing arguments.\n");
        fflush(stdout);
    }
    err = init_arg(argc, argv, &arg);
    if (err) {
        if (myid == 0) {
            printf("Failed to parse arguments. EXIT %d.\n", err);
            exit(-1);
        }
    }
    if (myid == 0) {
        printf("Start: GID %d, PID %d\nEND:GID %d, PID %d\n",
                arg.sgid, arg.spid, arg.egid, arg.epid);
        printf("===================\n");
        fflush(stdout);
    }
    if (myid == 0) {
        printf("Start computing interval.\n");
        fflush(stdout);
    }
    /* Calculating invervals for each record file. */
    err = calc_interval(&arg);
    if (err) {
        if (myid == 0) {
            printf("Failed to calculate interval values. EXIT %d.\n", err);
            exit(-1);
        }
    }

    free(arg.cpus);
    free(arg.ranks);
    free(arg.hosts[0]);
    free(arg.hosts);

    MPI_Finalize();

    return 0;
}
