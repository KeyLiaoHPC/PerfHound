#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#define NORM_SIGMA 0.016666667
#define NORM_CUT 0.1

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

    gsl_rng_env_setup();
    r = gsl_rng_alloc(T);
    get_urandom(&seed);
    gsl_rng_set(r, seed);


    sigma = 3 * NORM_SIGMA; // 3sigma for +-5% of 99.9% ndoes.
    ucut = 0 + NORM_CUT;  // Cutoff at +NORM_CUT
    dcut = 0 - NORM_CUT;  // Cutoff at -NORM_CUT

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

int
main(int argc, char **argv) {
    int nnodes[6] = {64,128,256,512,1024,2048};
    int i, j, ng = 6;
    float *pnode, *ptr;
    char name[256];

    for (i = 0; i < ng; i ++) {
        ptr = (float *)realloc((void*)pnode, nnodes[i] * sizeof(float));
        pnode = ptr;
        get_nodevar(pnode, nnodes[i]);

        FILE *fp;
        sprintf(name, "node_perf_%d.log", nnodes[i]);
        fp = fopen(name, "w");
        for (j = 0; j < nnodes[i]; j ++) {
            fprintf(fp, "%f\n", pnode[j]);
        }
        fclose(fp);
    } 
    free(pnode);

    return 0;
}