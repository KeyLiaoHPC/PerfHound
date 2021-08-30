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
#define NRUN 1000
#define NP_NODE 40
#define NNODE 100
#define SIGMA 0.016666667
#define NORM_CUT 0.1



int get_urandom(uint64_t *seed) {
    int i;
    FILE *fp = fopen("/dev/urandom", "r");
    if (fp == NULL) {
        printf("Failed to open random file.\n");
        return 1;
    } 
    uint64_t randNums[50];
    double x;

    fread(seed, sizeof(uint64_t), 1, fp); 

    fclose(fp);

    return 0;
}

int
main(int argc, char **argv){
    const gsl_rng_type * T;
    uint64_t seed;
    gsl_rng * r;

    float stdtime = Lf * Tf1 + Lf * Tf3 + Lf * Lk * Tk1 + Lf * Lk * Tk3 + Lf * Lk * Li * Ti;
    float tf1, tf3, tk1, tk3, ti;
    float tau_f, tau_k, tau_i;
    float node_perf[NNODE];
    // printf("Theoretical execution time: %f seconds \n", stdtime);
    int i, j;
#ifndef USE_MPI
    FILE *fp = fopen("data.log", "w");
    int njob = NP;

#else
    int nproc, myrank, err;
    FILE *fp;
    err = MPI_Init(NULL, NULL);
    if (err) {
        printf("Failed to initilize MPI environemnt.\n");

        exit(-1);
    }
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    
    if (nproc > NP) {
        printf("Too many MPI ranks, nrank must be less than nrank being simulated: \n", NP);
        MPI_Finalize();
        exit(-1);
    }

    if (myrank == 0) {
        FILE *fp = fopen("data.log", "w");
    }

    int njob = NP / nproc;
    int modu = NP % nproc;
    njob = (NP - modu) / nproc;
   
    if (myrank == 0) {
        printf("# of simulating rank for other process: %d\n", njob);
    }
    if (modu != 0 && myrank < modu) {
        // 除不尽
        njob += 1;
    }
    if (myrank == 0) {
        printf("# of simulating rank for rank 0-%d: %d\n", modu-1, njob);
        fflush(stdout);
        fp = fopen("data.log", "w");
    }
    
#endif // #ifndef USE_MPI


    gsl_rng_env_setup();

    T = gsl_rng_ranlux389;
    r = gsl_rng_alloc(T);

    // Node performance variation
    get_urandom(&seed);
    gsl_rng_set(r, seed);
    do {
        int n;
        float sigma, ucut, dcut;

        sigma = 3 * SIGMA; // 3sigma for +-5%
        ucut = 0 + NORM_CUT;
        dcut = 0 - NORM_CUT;
        if (NNODE == 1) {
            node_perf[0] = 1; 
        } else {
            for (n = 0; n < NNODE; n ++) {
                node_perf[n] = gsl_ran_gaussian(r, sigma);
                // Cut off for +-10%
                node_perf[n] = node_perf[n] > ucut? ucut: node_perf[n];
                node_perf[n] = node_perf[n] < dcut? dcut: node_perf[n];
            } 
        }
    } while(0)

    for (i = 0; i < NRUN; i++) {
        float time = 0, max_time = 0;
        float tau_i, tau_k, tau_f;
        for (j = 0; j < njob; j++) {
            time = 0;
            get_urandom(&seed);
            gsl_rng_set(r, seed);
            //double u = gsl_ran_gaussian(r, 0.025);
            //double tau_f1 = gsl_ran_pareto(r, 10, 1);
            //double tau_f3 = gsl_ran_pareto(r, 10, 1);
            //double tau_i = gsl_ran_pareto(r, 10, 1);
            int li, lf, lk;

            for (lf = 0; lf < Lf; lf ++) {
                tau_k = 0;
                for (lk = 0; lk < Lk; lk ++) {
                    tau_i = 0;
                    for (li = 0; li < Li; li ++) {
                        tau_i += gsl_ran_pareto(r, 10, Ti);
                    }
                    //tau_i = Li * Ti;
                    tau_k += gsl_ran_pareto(r, 10, Tk1);
                    tau_k += tau_i;
                    tau_k += Tk3;
                }
                tau_f = 0;
                tau_f += gsl_ran_pareto(r, 10, Tf1);
                tau_f += tau_k;
                tau_f += Tf3;

                time += tau_f;
            }
            max_time = time > max_time? time: max_time;
        }
#ifndef USE_MPI
        printf("%d\n", i);
        fprintf(fp, "%.12f\n", time);
#else
        float final_time;
        MPI_Reduce(&max_time, &final_time, 1, MPI_FLOAT, MPI_MAX, 0, MPI_COMM_WORLD);

        if (myrank == 0) {
            printf("%d\n", i);
            fprintf(fp, "%.12f\n", final_time);
        }
#endif // #ifndef USE_MPI
    }

#ifndef USE_MPI
    fclose(fp);
#else
    if (myrank == 0) {
        fclose(fp);
    }
#endif // #ifndef USE_MPI
    gsl_rng_free (r);

    MPI_Finalize();
    return 0;
}
