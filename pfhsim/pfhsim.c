#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <mpi.h>

/**
 * Pareto
 * K =      50           40         30      20      10
 * 0.90 - 1.0470    -   1.0592  -   1.0792  1.1210  1.2592
 * 0.95 - 1.0616    -   1.0777  -   1.1052  1.1606  1.3494
 * 0.99 - 1.0958    -   1.1225  -   1.1659  1.2583  1.5802
 * 0.999 - 1.1495   -   1.1893  -   1.2664  1.4074  1.9809
 */

#define Tf1 10
#define Tf3 0
#define Tk1 0.1 
#define Tk3 0
#define Ti  0.0001
#define Lf  10
#define Lk  100
#define Li  1000

#ifndef NRUN
#define NRUN 500
#endif

#ifndef NP_NODE
#define NP_NODE 64               // # of cores per node.
#endif

#ifndef NNODE
#define NNODE 512                 // # of nodes.
#endif

#define NORM_SIGMA 0.016666667
#define NORM_CUT 0.1

#ifndef PARETO_K
#define PARETO_K 10
#endif

#define SYSVAR_P 5              // Invoking period (sec) of os noise.
#define SYSVAR_T 0.2         // Time(sec) consumed by the os noise.

int nproc, myrank;


int 
get_urandom(uint64_t *x) {
    FILE *fp = fopen("/dev/urandom", "r");
    if (fp == NULL) {
        printf("Failed to open random file.\n");
        return 1;
    } 

    fread(x, sizeof(uint64_t), 1, fp); 

    fclose(fp);

    return 0;
}

/**
 * 
 * Node performance variation
 * @param pnode  pointers to node var perf factor
 * @param n      # of nodes.
 * @return int 
 */
int
get_nodevar(float *pnode, int n) {
    if (n == 0) {
        return 0;
    }
    int i;
    float sigma, ucut, dcut;
    const gsl_rng_type *T = gsl_rng_ranlux389;
    uint64_t seed;
    gsl_rng * r;

#ifdef DEBUG
    printf("Initializing random generator.\n");
    fflush(stdout);
#endif // #ifdef DEBUG

    gsl_rng_env_setup();
    r = gsl_rng_alloc(T);
    get_urandom(&seed);
    gsl_rng_set(r, seed);


    sigma = 3 * NORM_SIGMA; // 3sigma for +-5% of 99.9% ndoes.
    ucut = 0 + NORM_CUT;  // Cutoff at +NORM_CUT
    dcut = 0 - NORM_CUT;  // Cutoff at -NORM_CUT

#ifdef DEBUG
    printf("Start genrating Gaussian noise.\n");
    fflush(stdout);
#endif // #ifdef DEBUG
    //if (n == 1) {
    //    // No variation for single node.
    //    pnode[0] = 0; 
    //} else {
        for (i = 0; i < n; i ++) {
            pnode[i] = gsl_ran_gaussian(r, sigma);
            pnode[i] = pnode[i] > ucut? ucut: pnode[i];
            pnode[i] = pnode[i] < dcut? dcut: pnode[i];
        } 
    //}
    gsl_rng_free(r);

    return 0;
}

/**
 * Add noise to simulated run time
 * @param p_alltime 
 * @param nnode 
 * @param ppn 
 * @param p 
 * @param t 
 * @param final_time 
 */
void
add_osnoise(float *p_alltime, int nnode, int ppn, float p, float t, float *final_time) {
    int simnp = NP_NODE * NNODE;
    int i, j, k, core;
    int nnoise, passed_noise;
    uint64_t r;
    float tmax, final = 0, node_off;
    float *ptr, *ptime;

    int myjob, njobp0, njobp1;
    int modu, real_nproc, off, err;
    MPI_Status mpistat;

    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    modu = nnode % nproc;
    njobp0 = (nnode - modu) / nproc;
    njobp1 = njobp0 + 1;
    if (njobp0 == 0) {
        real_nproc = modu;
    } else {
        real_nproc = nproc;
    }
    // allocate sim rank to mpi rank;
    if (myrank < modu) {
        // 除不尽
        myjob = njobp1;
    } else {
        myjob = njobp0;
    }
    if (myjob) {
        ptime = (float*)malloc(myjob * ppn * sizeof(float));
    }

    if (myrank == 0) {
        for (i = 1; i < real_nproc; i ++) {
            if (i < modu) {
                off = njobp1 * i * ppn;
            } else {
                off = ppn * (modu * njobp1 + (i - modu) * njobp0);
            }
            if (i < modu) {
                MPI_Send(p_alltime + off, ppn * njobp1, MPI_FLOAT, i, i, MPI_COMM_WORLD);
            } else {
                MPI_Send(p_alltime + off, ppn * njobp0, MPI_FLOAT, i, i, MPI_COMM_WORLD);
            }
            
        }
        for (j = 0; j < myjob * ppn; j ++) {
                ptime[j] = p_alltime[j];
        }
    } else {
        if (myjob != 0) {
            MPI_Recv(ptime, ppn * myjob, MPI_FLOAT, 0, myrank, MPI_COMM_WORLD, &mpistat);
        }
    }


    for (i = 0; i < myjob; i ++) {
        // hypothesis: each node start at a random time in a period.
        get_urandom(&r);
        // How many seconds from the starting point to next interrupt.
        ptr = ptime + ppn * i;
        node_off = p * ((float)r / (float)UINT64_MAX);
        tmax = 0;
        for (j = 0; j < ppn; j ++) {
            tmax = tmax < ptr[j] ? ptr[j] : tmax;
        }

        nnoise = (tmax - node_off) / p;
        passed_noise = 0;

        while(nnoise > passed_noise){
            for (k = 0; k < nnoise - passed_noise; k ++) {
                get_urandom(&r);
                core = r % ppn;
                ptr[core] += t;
                tmax = tmax < ptr[core]? ptr[core] : tmax;
            }

            passed_noise += k;
            nnoise = (tmax - node_off) / p;
        }
        final = final < tmax ? tmax : final;
        
    }

    MPI_Reduce(&final, final_time, 1, MPI_FLOAT, MPI_MAX, 0, MPI_COMM_WORLD);
}

int
main(int argc, char **argv){
    // Variates for random samples
    const gsl_rng_type *T;
    gsl_rng *r;
    uint64_t seed;
    // Variates for simulting
    int simnp = NP_NODE * NNODE;
    
    float stdtime = Lf * Tf1 + Lf * Tf3 + Lf * Lk * Tk1 + Lf * Lk * Tk3 + Lf * Lk * Li * Ti;
    float tf1, tf3, tk1, tk3, ti;
    float tau_f, tau_k, tau_i;
    float arr_tau_f[Lf], arr_tau_k[Lf][Lk], arr_tau_i[Lf][Lk][Li];
    float node_perf[NNODE];
    int *p_allnjob; // ADJ factor for node performance variation.
    int i, j;
    int err, simrank_st, simrank_en;
    FILE *fp;

    err = MPI_Init(NULL, NULL);
    if (err) {
        printf("Failed to initilize MPI environemnt.\n");

        exit(-1);
    }
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    if (simnp == 0) {
        printf("# of ranks = 0. EXIT.\n");
        return 0;
    }
    
    if (nproc > simnp) {
        printf("Too many MPI ranks, nrank must <= %d \n", simnp);
        MPI_Finalize();
        exit(-1);
    }

    if (myrank == 0) {
        FILE *fp = fopen("data.log", "w");
    }

    int myjob, njobp0, njobp1;
    int modu = simnp % nproc;
    njobp0 = (simnp - modu) / nproc;
    njobp1 = njobp0 + 1;
   
    if (myrank == 0) {
        printf("njobp0: %d, njobp1: %d\n", njobp0, njobp1);
    }
    // allocate sim rank to mpi rank;
    if (myrank < modu) {
        // 除不尽
        myjob = njobp1;
    } else {
        myjob = njobp0;
    }
#ifdef DEBUG
    if (myrank == 0) {
        printf("nproc = %d, simnp = %d, modu = %d \n", nproc, simnp, modu);
        fflush(stdout);
    }
#endif // #ifdef DEBUG
    if (myrank == 0) {
        fp = fopen("data.log", "w");
    }
    // allocate node id to each rank
    p_allnjob = (int *)malloc(nproc * sizeof(int));
    if (p_allnjob == NULL) {
        printf("Rank %d failed to alloc p_allnjob.\n", myrank);
        fflush(stdout);
        exit(-1);
    }
    for (i = 0; i < modu; i ++) {
        p_allnjob[i] = njobp1;
    }
    for (i = modu; i < nproc; i ++) {
        p_allnjob[i] = njobp0;
    }
    // node[0-(NNODE-1)]

    if (myrank < modu) {
        // The rank which has one more job.
        simrank_st = myrank * njobp1;
    } else {
        simrank_st = modu * njobp1 + (myrank - modu) * njobp0;
    }
    simrank_en = simrank_st + myjob - 1;
#ifdef DEBUG
    do {
        int flag = 1;
        if (myrank == 0) {
            printf( "rank=%d, simrank=%d-%d\n", myrank, simrank_st, simrank_en);
            fflush(stdout);
        } else {
            MPI_Status mpistat;
            MPI_Recv(&flag, 1, MPI_INT, myrank-1, myrank-1, MPI_COMM_WORLD, &mpistat);
            printf( "rank=%d, simrank=%d-%d\n", myrank, simrank_st, simrank_en);
            fflush(stdout);
        }
        if (myrank + 1 < nproc) {
            MPI_Send(&flag, 1, MPI_INT, myrank + 1, myrank, MPI_COMM_WORLD);
        }
    } while (0);
#endif // #ifdef DEBUG

    MPI_Barrier(MPI_COMM_WORLD);

    if (myrank == 0) {
        char name[256];
        FILE *fnode;

        sprintf(name, "./node_perf_%d.log", NNODE);
        fnode = fopen(name, "r");
        if (fnode == NULL) {
            get_nodevar(node_perf, NNODE);
            printf("Generating node variation.\n");
            fflush(stdout);
        } else {
            printf("Using existed node variation factor. %s\n", name);
            for (i = 0; i < NNODE; i ++) {
                fscanf(fnode, "%f\n", node_perf+i);
#ifdef DEBUG
                printf("%d, %f\n", i, node_perf[i]);
                fflush(stdout);
#endif
            }
            fclose(fnode);
        }
        printf("Start simulation.\nPerformance factor for node 0-%d: ", NNODE-1);
        for (i = 0; i < NNODE; i ++) {
            printf("%f, ", node_perf[i]);
        }
        printf("\n");
        printf("Theoretical execution time: %f seconds \n", stdtime);
        printf("NP_NODE=%d, NNODE=%d, PARETO_K=%d\n", NP_NODE, NNODE, PARETO_K);
        fflush(stdout);
    } 

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Bcast(node_perf, NNODE, MPI_FLOAT, 0, MPI_COMM_WORLD);
    
    
    gsl_rng_env_setup();
    T = gsl_rng_ranlux389;
    r = gsl_rng_alloc(T);

    
#ifdef OSNOISE
        // arrays for OS noise
        float *p_alltime, *p_mytime;
        int *p_displ;
        p_alltime = (float *)malloc(simnp * sizeof(float));
        p_mytime = (float *)malloc(myjob * sizeof(float));
        p_displ = (int *)malloc(nproc * sizeof(int));
        if (p_alltime == NULL || p_mytime == NULL || p_displ == NULL) {
            printf("Failed to allocate os noise helper arrays.\n");
            exit(-1);
        }
        
        p_displ[0] = 0;
        for (i = 1; i < nproc; i ++) {
            p_displ[i] = p_displ[i-1] + p_allnjob[i-1];
        }
#endif // #ifdef OSNOISE
    /* Start simulation */
    for (i = 0; i < NRUN; i++) {
        float time = 0, max_time = 0;
        float tau_i, tau_k, tau_f;
        int li, lf, lk, simnode;

        get_nodevar(node_perf, NNODE);

        for (j = 0; j < myjob; j++) {
            register float s = 1.0;
            simnode = (simrank_st + j) / NP_NODE;
            s = 1 + node_perf[simnode];

            ti = (float)Ti / s;
            tk1 = (float)Tk1 / s;
            tk3 = (float)Tk3 / s;
            tf1 = (float)Tf1 / s;
            tf3 = (float)Tf3 / s;

            get_urandom(&seed);
            gsl_rng_set(r, seed);

            for (lf = 0; lf < Lf; lf ++) {
#ifdef NO_FVAR
                arr_tau_f[lf] = tf1; //gsl_ran_pareto(r, PARETO_K, 1);
#else
                arr_tau_f[lf] = tf1 * gsl_ran_pareto(r, PARETO_K, 1);
#endif
                for (lk = 0; lk < Lk; lk ++) {
#ifdef NO_KVAR
                    arr_tau_k[lf][lk] = tk1; //gsl_ran_pareto(r, PARETO_K, 1);
#else
                    arr_tau_k[lf][lk] = tk1 * gsl_ran_pareto(r, PARETO_K, 1);
#endif
                    for (li = 0; li < Li; li ++) {
#ifdef NO_IVAR
                        arr_tau_i[lf][lk][li] = ti; //gsl_ran_pareto(r, PARETO_K, 1);
#else                    
                        arr_tau_i[lf][lk][li] = ti * gsl_ran_pareto(r, PARETO_K, 1);
#endif
                    }
                }
            }

            time = 0;
            for (lf = 0; lf < Lf; lf ++) {
                tau_k = 0;
                for (lk = 0; lk < Lk; lk ++) {
                    tau_i = 0;
                    for (li = 0; li < Li; li ++) {
                        tau_i += arr_tau_i[lf][lk][li];
                    }
                    tau_k += tau_i;
                    tau_k += arr_tau_k[lf][lk];
                    tau_k += tk3;
                }
                time += arr_tau_f[lf];
                time += tau_k;
                time += tf3;
            }

#ifdef OSNOISE
            p_mytime[j] = time;
#else
            max_time = time > max_time? time: max_time;
#endif // #ifdef OSNOISE
        }
        float final_time;
#ifdef OSNOISE
        //MPI_Gatherv(p_mytime, myjob, MPI_FLOAT, 
        //            p_alltime, p_allnjob, p_displ, MPI_FLOAT, 0, MPI_COMM_WORLD);
        if (myrank == 0) {
            int iii, in, kkk;
            MPI_Status mpistat;
            for (iii = 0; iii < myjob; iii++) {
                p_alltime[iii] = p_mytime[iii];
            }
            for (in = 1; in < nproc; in ++) {

                MPI_Recv(p_alltime + p_displ[in], p_allnjob[in], MPI_FLOAT, in, in, MPI_COMM_WORLD, &mpistat);
            }
        } else {
            int in;
            MPI_Send(p_mytime, myjob, MPI_FLOAT, 0, myrank, MPI_COMM_WORLD);
        }
        add_osnoise(p_alltime, NNODE, NP_NODE, SYSVAR_P, SYSVAR_T, &final_time);
#else
        MPI_Reduce(&max_time, &final_time, 1, MPI_FLOAT, MPI_MAX, 0, MPI_COMM_WORLD);
#endif // #ifdef OSNOISE

        if (myrank == 0) {
            printf("%d, %f\n", i, final_time);
            fprintf(fp, "%.12f\n", final_time);
            fflush(stdout);
        }

    }

#ifdef OSNOISE
    free(p_alltime);
    free(p_mytime);
    free(p_displ);
#endif

    if (myrank == 0) {
        fclose(fp);
    }
    free(p_allnjob);

    gsl_rng_free(r);
    MPI_Finalize();

    return 0;
}
