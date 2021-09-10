#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#define Tf1 10
#define Tf3 0
#define Tk1 0.1 
#define Tk3 0
#define Ti  0.0001
#define Lf  10
#define Lk  100
#define Li  1000

#ifndef NRUN
#define NRUN 1000
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

#define PARETO_CUT 2

#define SYSVAR_P 5              // Invoking period (sec) of os noise.
#define SYSVAR_T 0.2         // Time(sec) consumed by the os noise.



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

int
main(int argc, char **argv){

    FILE *fp;
    char fname[1024];
    int nsteps[4] = {1, 100, 1000, 10000};

    const gsl_rng_type *T = gsl_rng_ranlux389;
    uint64_t seed;
    gsl_rng * r;
    float x[NRUN];

    gsl_rng_env_setup();
    r = gsl_rng_alloc(T);
    get_urandom(&seed);
    gsl_rng_set(r, seed);

    for (int i = 0; i < 4; i ++) {
        int nrep = nsteps[i];
        if (nrep == 0) nrep = 1;
        sprintf(fname, "rep%d.log", nrep);
        fp = fopen(fname, "w");
        for (int j = 0; j < NRUN; j ++) {
            x[j] = 0;
            get_urandom(&seed);
            gsl_rng_set(r, seed);
            for (int k = 0; k < nrep; k ++) {
                // x[j] += Ti * gsl_ran_pareto(r, 3, 1);
                // x[j] += Ti * (gsl_ran_gaussian(r, 0.15) + 1.5);
                x[j] += Ti * (gsl_ran_bernoulli(r, 0.4) * 1.25 + 1);
            }
            x[j] /= nrep;
            fprintf(fp, "%.12f\n", x[j]);
        }
        printf("%d\n", i);
        fclose(fp);
    }


    gsl_rng_free(r);
    return 0; 
}


