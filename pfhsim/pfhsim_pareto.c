#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#define Tf1 50
#define Tf3 50
#define Tk1 0.2 
#define Tk3 0.1
#define Ti  0.0002
#define Lf  10
#define Lk  100
#define Li  1000
#define NRUN 500
#define NP_NODE 64               // # of cores per node.
#define NNODE 4                 // # of nodes.
#define NORM_SIGMA 0.016666667
#define NORM_CUT 0.1
#define PARETO_K 40
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

    FILE *fp = fopen("pareto.log", "w");

    int i;
    float sigma, ucut, dcut;
    const gsl_rng_type *T = gsl_rng_ranlux389;
    uint64_t seed;
    gsl_rng * r;
    float x[100000];

    gsl_rng_env_setup();
    r = gsl_rng_alloc(T);
    get_urandom(&seed);
    gsl_rng_set(r, seed);

    for (i = 0; i < 100000; i ++) {
        x[i] = gsl_ran_pareto(r, 1, 1);
        fprintf(fp, "%f\n", x[i]);
    }
    fclose(fp);
    return 0; 
}


