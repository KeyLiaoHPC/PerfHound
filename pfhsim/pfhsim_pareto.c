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

    int i;
    const gsl_rng_type *T = gsl_rng_ranlux389;
    uint64_t seed;
    gsl_rng * r;
    float x[100000], tau_i_arr[100000], tau_k_arr[100000], tau_f_arr[100000];

    gsl_rng_env_setup();
    r = gsl_rng_alloc(T);
    get_urandom(&seed);
    gsl_rng_set(r, seed);

    for (i = 0; i < 100000; i ++) {
        x[i] = gsl_ran_pareto(r, PARETO_K, 1);
        x[i] = x[i] > PARETO_CUT ? PARETO_CUT : x[i];
        tau_i_arr[i] = Ti * x[i];
        tau_k_arr[i] = Tk1 * x[i];
        tau_f_arr[i] = Tf1 * x[i];
    }

    sprintf(fname, "k%dti.log", PARETO_K);
    fp = fopen(fname, "w");
    for (i = 0; i < 100000; i ++) {
        fprintf(fp, "%.12f\n", tau_i_arr[i]);
    }
    fclose(fp);
    sprintf(fname, "k%dtk.log", PARETO_K);
    fp = fopen(fname, "w");
    for (i = 0; i < 100000; i ++) {
        fprintf(fp, "%.12f\n", tau_k_arr[i]);
    }
    fclose(fp);
    sprintf(fname, "k%dtf.log", PARETO_K);
    fp = fopen(fname, "w");
    for (i = 0; i < 100000; i ++) {
        fprintf(fp, "%.12f\n", tau_f_arr[i]);
    }
    fclose(fp);
    sprintf(fname, "k%ds.log", PARETO_K);
    fp = fopen(fname, "w");
    for (i = 0; i < 100000; i ++) {
        fprintf(fp, "%.12f\n", x[i]);
    }
    fclose(fp);

    gsl_rng_free(r);
    return 0; 
}


