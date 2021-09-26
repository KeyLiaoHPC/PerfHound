
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
            printf("%d\n", mod*(n+1)+(i-mod)*n);
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
        printf("%d: %d \n", myid, arg->nf);
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

void
step_to(char *str, char ch, char **p) {
    int len = strlen(str);
    for (int i = 0; i < len; i ++) {
        if (str[i] == ch) {
            *p = str + i;
            break;
        }
    }
}

int
calc_interval(arg_t *arg) {
    long fsize;
    int ncol, gid, pid, nev, id, myid;
    uint64_t cy, ns;
    double uval;
    char fname[10240];
    FILE *fp;
    char *recstr = NULL, *pst = NULL, *pen = NULL;
    uint64_t *evs;

    MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    sprintf(fname, "%s/%s/r%dc%d.csv",
            arg->ipath, arg->hosts[0], arg->ranks[0], arg->cpus[0]);
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
    if (nev) {
        evs = (uint64_t *)malloc(nev*sizeof(uint64_t));
    }

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
        fread(recstr, 1, fsize, fp);
        recstr[fsize] = '\0';
        pst = recstr;
        pen = pst;
        fclose(fp);

        /* File for output. */
        sprintf(fname, "%s/r%dc%d.csv", 
                arg->opath, arg->ranks[i], arg->cpus[i]);
        printf("Rank %d: Data will write to %s\n", myid, fname);
        fp = fopen(fname, "w");
        if (fp == NULL) {
            return errno;
        }
        step_to(recstr, '\n', &pen);
        pst = pen + 1;
        printf("%d %d %d %d\n", *pen, *(pen+1), *(pen+2), *(pen+3));
        fflush(stdout);

        id = 0;
        /* Start calculating */
        while (*pst) {
            // Search for start line
            pen = pst;
            step_to(pst, ',', &pen);
            *pen = '\0';
            gid = strtol(pst, NULL, 10);
            //printf("%d\n", gid);

            if (gid != arg->sgid) {
                step_to(pen+1, '\n', &pen);
                pst = pen + 1;
                continue;
            }
            pst = pen + 1;
            step_to(pst, ',', &pen);
            *pen = '\0';
            pid = strtol(pst, NULL, 10);
            if (pid != arg->spid) {
                step_to(pen+1, '\n', &pen);
                pst = pen + 1;
                continue;
            }
            // Extract start values.
            pst = pen + 1;
            step_to(pst, ',', &pen);
            *pen = '\0';
            cy = strtoull(pst, NULL, 10);
            step_to(pst, ',', &pen);
            *pen = '\0';
            pst = pen + 1;
            ns = strtoull(pst, NULL, 10);
            step_to(pst, ',', &pen);
            *pen = '\0';
            pst = pen + 1;
            uval = strtod(pst, NULL);
            if (nev) {
                for (int j = 0; j < nev - 1; j ++) {
                    step_to(pst, ',', &pen);
                    *pen = '\0';
                    pst = pen + 1;
                    evs[j] = strtoull(pst, NULL, 10);
                }
                pst = pen + 1;
                step_to(pst, '\n', &pen);
                *pen = '\0';
                evs[nev-1] = strtoull(pst, NULL, 10);
            }
            // Search for end line.
            pst = pen + 1;
            step_to(pst, ',', &pen);
            *pen = '\0';
            gid = strtol(pst, NULL, 10);
            if (gid != arg->egid) {
                step_to(pen+1, '\n', &pen);
                pst = pen + 1;
                continue;
            }
            pst = pen + 1;
            step_to(pst, ',', &pen);
            *pen = '\0';
            pid = strtol(pst, NULL, 10);
            if (pid != arg->epid) {
                step_to(pen+1, '\n', &pen);
                pst = pen + 1;
                continue;
            }
            // Extract end values.
            pst = pen + 1;
            step_to(pst, ',', &pen);
            *pen = '\0';
            cy = strtoull(pst, NULL, 10) - cy;
            step_to(pst, ',', &pen);
            *pen = '\0';
            pst = pen + 1;
            ns = strtoull(pst, NULL, 10) - ns;
            step_to(pst, ',', &pen);
            *pen = '\0';
            pst = pen + 1;
            if (nev) {
                for (int j = 0; j < nev - 1; j ++) {
                    step_to(pst, ',', &pen);
                    pst = pen + 1;
                    evs[j] = strtoull(pst, NULL, 10) - evs[j];
                }
                pst = pen + 1;
                step_to(pst, '\n', &pen);
                evs[nev-1] = strtoull(pst, NULL, 10) - evs[nev-1];
            }
            // Write out.
            fprintf(fp, "%d,%s,%d,%d,%lu,%lu,%.15f", 
                    id, arg->hosts[i], arg->ranks[i], arg->cpus[i], cy, ns, uval);
            if (nev) {
                for (int j = 0; j < nev; j ++) {
                    fprintf(fp, ",%lu", evs[j]);
                }
            }
            fprintf(fp, "\n");

            // Next couple
            pst = pen + 1;
        }
        fflush(fp);
        fclose(fp);
    }
    if (nev) {
        free(evs);
    }
    free(recstr);
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
