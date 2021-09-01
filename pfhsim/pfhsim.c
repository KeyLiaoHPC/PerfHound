#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <mpi.h>

#define Tf1 50
#define Tf3 50
#define Tk1 0.2 
#define Tk3 0.1
#define Ti  0.0002
#define Lf  10
#define Lk  100
#define Li  1000
#define NRUN 500
#define NP_NODE 40               // # of cores per node.
#define NNODE 32                 // # of nodes.
#define NORM_SIGMA 0.016666667
#define NORM_CUT 0.1
#define PARETO_K 49
#define SYSVAR_P 2              // Invoking period (sec) of os noise.
#define SYSVAR_T 0.0002         // Time(sec) consumed by the os noise.



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
    if (n == 1) {
        // No variation for single node.
        pnode[0] = 0; 
    } else {
        for (i = 0; i < n; i ++) {
            pnode[i] = gsl_ran_gaussian(r, sigma);
            pnode[i] = pnode[i] > ucut? ucut: pnode[i];
            pnode[i] = pnode[i] < dcut? dcut: pnode[i];
        } 
    }
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
    float tmax, off, final;

    final = 0;

    for (i = 0; i < nnode; i ++) {
        // hypothesis: each node start at a random time in a period.
        get_urandom(&r);
        // How many seconds from the starting point to next interrupt.
        off = p * ((float)r / (float)UINT64_MAX);
        tmax = 0;
        for (j = i * ppn; j < (i + 1) * ppn; j ++) {
            tmax = tmax < p_alltime[j] ? p_alltime[j] : tmax;
        }
        nnoise = (tmax - off) / p;
        passed_noise = 0;

        do {
            for (k = 0; k < nnoise - passed_noise; k ++) {
                get_urandom(&r);
                core = r % ppn;
                p_alltime[i*nnode+core] += t;
                tmax = tmax < p_alltime[i*nnode+core]? p_alltime[i*nnode+core] : tmax;
            }
            passed_noise += k;
            nnoise = (tmax - off) / p;
        } while(nnoise > passed_noise);
        final = final < tmax ? tmax : final;
    }

    *final_time = final;
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
    int *pnode_id, *p_allnjob; // ADJ factor for node performance variation.
    // printf("Theoretical execution time: %f seconds \n", stdtime);
    int i, j;
    int nproc, myrank, err, simrank_st, simrank_en;
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
    pnode_id = (int *)malloc(myjob*sizeof(int));
    p_allnjob = (int *)malloc(nproc * sizeof(int));
    if (pnode_id == NULL || p_allnjob == NULL) {
        printf("Rank %d failed to alloc pnode_id.\n", myrank);
        fflush(stdout);
        exit(-1);
    }
    for (i = 1; i < modu; i ++) {
        p_allnjob[i] = njobp1;
    }
    for (i = modu; i < nproc; i ++) {
        p_allnjob[i] = njobp0;
    }
    // node[0-(NNODE-1)]

    if (myrank < modu) {
        // The rank which has one more job.
        simrank_st = myrank * njobp1;
        simrank_en = simrank_st + myjob - 1;
    } else {
        simrank_st = modu * njobp1 + (myrank - modu) * njobp0;
        simrank_en = simrank_st + myjob - 1;
    }
    for (i = 0; i < myjob; i ++) {
        pnode_id[i] = (simrank_st + i) / NP_NODE;
#ifdef DEBUG
        do {
            int flag = 1;
            if (myrank == 0) {
                printf( "rank=%d, simrank=%d, node_id=%d\n", 
                        myrank, simrank_st + i, pnode_id[i]);
                fflush(stdout);
            } else {
                MPI_Status mpistat;
                MPI_Recv(&flag, 1, MPI_INT, myrank-1, myrank-1, MPI_COMM_WORLD, &mpistat);
                printf( "rank=%d, simrank=%d, node_id=%d\n",
                        myrank, simrank_st + i, pnode_id[i]);
                fflush(stdout);
            }
            if (myrank + 1 < nproc) {
                MPI_Send(&flag, 1, MPI_INT, myrank + 1, myrank, MPI_COMM_WORLD);
            }
            MPI_Barrier(MPI_COMM_WORLD);
        } while (0);
#endif // #ifdef DEBUG
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (myrank == 0) {
        char name[256];
        FILE *fnode;

        sprintf(name, "node_perf_%d.log", NNODE);
        fnode = fopen(name, "r");
        if (fnode == NULL) {
            get_nodevar(node_perf, NNODE);
        } else {
            for (i = 0; i < NNODE; i ++) {
                fscanf(fnode, "%f\n", node_perf[i]);
            }
            fclose(fnode);
        }
    } 


    MPI_Bcast(node_perf, NNODE, MPI_FLOAT, 0, MPI_COMM_WORLD);
    
    
    gsl_rng_env_setup();
    T = gsl_rng_ranlux389;
    r = gsl_rng_alloc(T);

    
#ifdef DEBUG
    MPI_Barrier(MPI_COMM_WORLD);
    if (myrank == 0) {
        printf("Start simulation.\nPerformance factor for node 0-%d: ", NNODE-1);
        for (i = 0; i < NNODE; i ++) {
            printf("%f, ", node_perf[i]);
        }
        printf("\n");
        fflush(stdout);
    }
#endif // #ifdef DEBUG


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
        for (i = 1; i <= modu; i ++) {
            p_displ[i] = p_displ[i-1] + njobp1;
            p_allnjob[i] = njobp1;
        }
        for (i = modu + 1; i < nproc; i ++) {
            p_displ[i] = p_displ[i-1] + njobp0;
            p_allnjob[i] = njobp0;
        }
#endif // #ifdef OSNOISE
    /* Start simulation */
    for (i = 0; i < NRUN; i++) {
        float time = 0, max_time = 0;
        float tau_i, tau_k, tau_f;
        int li, lf, lk;

        for (j = 0; j < myjob; j++) {
            register float s = 1.0;
            s = 1 + node_perf[pnode_id[j]];

            ti = (float)Ti / s;
            tk1 = (float)Tk1 / s;
            tk3 = (float)Tk3 / s;
            tf1 = (float)Tf1 / s;
            tf3 = (float)Tf3 / s;

            get_urandom(&seed);
            gsl_rng_set(r, seed);

            for (lf = 0; lf < Lf; lf ++) {
#ifndef NO_FVAR
                arr_tau_f[lf] = tf1 * gsl_ran_pareto(r, PARETO_K, 1);
#endif
                for (lk = 0; lk < Lk; lk ++) {
#ifndef NO_KVAR
                    arr_tau_k[lf][lk] = tk1 * gsl_ran_pareto(r, PARETO_K, 1);
#endif
                    for (li = 0; li < Li; li ++) {
#ifndef NO_IVAR
                        arr_tau_i[lf][lk][li] = ti * gsl_ran_pareto(r, PARETO_K, 1);
#endif
                    }
                }
            }

            time = 0;
            for (lf = 0; lf < Lf; lf ++) {
                tau_k = 0;
                for (lk = 0; lk < Lk; lk ++) {
#ifndef NO_IVAR
                    tau_i = 0;
                    for (li = 0; li < Li; li ++) {
                        tau_i += arr_tau_i[lf][lk][li];
                    }
#else
                    tau_i = Li * ti;
#endif
                    tau_k += tau_i;
#ifndef NO_KVAR
                    tau_k += arr_tau_k[lf][lk];
#else
                    tau_k += tk1;
#endif
                    tau_k += tk3;
                }
                tau_f = 0;
#ifndef NO_FVAR
                tau_f += arr_tau_f[lf];
#else
                tau_f += tf1;
#endif
                tau_f += tau_k;
                tau_f += tf3;

                time += tau_f;
            }
#ifdef OSNOISE
            p_mytime[j] = time;
#else
            max_time = time > max_time? time: max_time;
#endif // #ifdef OSNOISE
        }
        float final_time;
#ifdef OSNOISE
        MPI_Gatherv(p_mytime, myjob, MPI_FLOAT, 
                    p_alltime, p_allnjob, p_displ, MPI_FLOAT, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        // Spreading again
        if (myrank == 0) {
            add_osnoise(p_alltime, NNODE, NP_NODE, SYSVAR_P, SYSVAR_T, &final_time);
        }
#else
        MPI_Reduce(&max_time, &final_time, 1, MPI_FLOAT, MPI_MAX, 0, MPI_COMM_WORLD);
#endif // #ifdef OSNOISE

        if (myrank == 0) {
            if (i) {
                int s = i;
                do {
                    printf("\b");
                    s /= 10;
                } while (s);
                fflush(stdout);
            }
            printf("%d", i);
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
    free(pnode_id);
    free(p_allnjob);

    gsl_rng_free(r);
    MPI_Finalize();

    return 0;
}
