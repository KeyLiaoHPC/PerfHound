#define _GNU_SOURCE
#define _ISOC11_SOURCE

#include <mpi.h>
#include <time.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <pfh_mpi.h>

#ifdef PAPI
#include <papi.h>
#endif

// PerfHound mode
#ifndef MODE
#define MODE TS
#endif

// name of test kernel, default is ADD
#ifndef KNAME
#define KNAME ADD
#endif

// number of test block
#ifndef ROUND
#define ROUND 0
#endif

// used for flush cache
#ifndef ARRLEN
#define ARRLEN 720896
#endif

// number of repeat run
#ifndef NMEASURE
#define NMEASURE 10000
#endif

#define _M2S(x) #x
#define M2S(x) _M2S(x)
#define _STRCAT(x, y) x##y
#define STRCAT(x, y) _STRCAT(x, y)
#define KERNEL(x) STRCAT(DEP_, x)

#define DEP_ADD "add %%r11, %%r12 \n\t" \
                "add %%r12, %%r11 \n\t"

#define DEP_MUL "imul %%r11, %%r12 \n\t" \
                "imul %%r12, %%r11 \n\t"

#define DEP_LDST "mov (%%rsp), %%r10 \n\t" \
                 "mov %%r10, (%%rsp) \n\t" 

#define DEP_FMA "vfmadd213pd %%xmm0, %%xmm1, %%xmm1 \n\t" \
                "vfmadd213pd %%xmm1, %%xmm0, %%xmm0 \n\t"

#define DEP_LD "mov (%%rsp), %%r10 \n\t" \
               "mov (%%rsp), %%r10 \n\t" 

#define DEP_ST "mov %%r10, (%%rsp) \n\t" \
               "mov %%r10, (%%rsp) \n\t" 

void flush_cache(double* arr1,  double* arr2, double* arr3, double scalar) {
    for (int i = 0; i < ARRLEN; ++i) {
        arr1[i] = arr2[i] * scalar + arr3[i];
    }
}

void one_round_test(double* arr, double scalar) {
    int idx = 0, measure_counter = 0;

    // register initialization
    asm volatile (
        "mov $1, %%r11 \n\t"
        "mov $0, %%r12 \n\t"
        "vpcmpeqw %%xmm0, %%xmm0, %%xmm0 \n\t"
        "vpsllq $54, %%xmm0, %%xmm0 \n\t"
        "vpsrlq $2, %%xmm0, %%xmm0 \n\t"
        "vmovaps %%xmm0, %%xmm1 \n\t"
        "movsd %%xmm0, %%xmm0 \n\t"
        "movsd %%xmm1, %%xmm1 \n\t"
        "mov (%%rsp), %%r10 \n\t"
        :
        :
        : "r10", "r11", "r12", "xmm0", "xmm1"
    );

    while ((measure_counter++) < NMEASURE) {
        pfhmpi_read(1, 1, 0);
#pragma GCC unroll 10240
        for (int i = 0; i < ROUND; ++i) {
            asm volatile (
                KERNEL(KNAME)
                :
                :
                : "r10", "r11", "r12", "xmm0", "xmm1", "memory"
            );
        }
        pfhmpi_read(1, 2, 0);
        flush_cache(arr1, arr2, arr3, scalar);
    }
}

int main(int argc, char** argv) {
    char dirname[256];
    char* op = M2S(KNAME);
    char* mode = M2S(MODE);

    MPI_Init(NULL, NULL);
#ifdef PAPI
    sprintf(dirname, "./data/papi_mpi/%s_%s_test_6248_20220222/round_%d", mode, op, ROUND);
#else
    sprintf(dirname, "./data/pfhd_mpi/%s_%s_test_6248_20220222/round_%d", mode, op, ROUND);
#endif

    // create directory
    if (pfhmpi_init(dirname)) { 
        printf("Failed at initailizing PerfHound.\n");
        exit(1);
    }
    pfhmpi_set_tag(1, 0, M2S(STRCAT(KNAME, _Test)));
    pfhmpi_set_tag(1, 1, M2S(STRCAT(KNAME, _Start)));
    pfhmpi_set_tag(1, 2, M2S(STRCAT(KNAME, _End)));

    if (strcmp(mode, "EVX") == 0) {
        pfhmpi_set_evt("inst_retired.any_p");
        pfhmpi_set_evt("uops_executed_port.port_0");
        pfhmpi_set_evt("uops_executed_port.port_1");
        pfhmpi_set_evt("uops_executed_port.port_2");
        pfhmpi_set_evt("uops_executed_port.port_4");
        pfhmpi_set_evt("uops_executed_port.port_5");
        pfhmpi_set_evt("uops_executed_port.port_6");
    }
    pfhmpi_commit();

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    // initial array
    struct timespec tv;
    double scalar, arr1[ARRLEN], arr2[ARRLEN], arr3[ARRLEN];
    clock_gettime(CLOCK_MONOTONIC, &tv);
    // srand(0);
    srand(tv.tv_nsec);
    scalar = ((rand() * 1.0) / RAND_MAX);
    for (int i = 0; i < ARRLEN; ++i) {
        arr1[i] = ((rand() * 1.0) / RAND_MAX);
        arr2[i] = ((rand() * 1.0) / RAND_MAX);
        arr3[i] = ((rand() * 1.0) / RAND_MAX);
    }
    flush_cache(arr1, arr2, arr3, scalar);
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    one_round_test(arr1, arr2, arr3, scalar);
    /*asm volatile (
        "mov %%r11, %0 \n\t"
        : "=r"(res)
        :
        : "r11", "memory"
    );
    printf("%d\n", res);*/

    MPI_Barrier(MPI_COMM_WORLD);
    pfhmpi_finalize();
    MPI_Finalize();
    return 0;
}
