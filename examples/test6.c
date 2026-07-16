#define _GNU_SOURCE
#define _ISOC11_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdarg.h>
#include <unistd.h>
#include <sched.h>
#include <string.h>
#include <mpi.h>
#include <ph_mpi.h>

#ifndef PH_DATA_ROOT
#define PH_DATA_ROOT "./ph_data/test6"
#endif

#define PHMPI 1
#include "ph_events.h"

#ifdef PAPI
#include <papi.h>
#endif


#ifndef NINS
#define NINS 100
#endif

#ifndef NMEASURE
#define NMEASURE 205
#endif

#ifndef KNAME
#define KNAME ADD
#endif

#ifndef MODE
#define MODE TS
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

int main(int argc, char** argv) {
    int64_t res;
    struct timespec tv;
    char* op = M2S(KNAME);
    char* mode = M2S(MODE);
    uint64_t volatile sec, nsec; // For warmup
    int idx = 0, measure_counter = 0;
    int64_t a[4] = {1, 2, 1, 1};

    MPI_Init(NULL, NULL);

    if (phmpi_init(PH_DATA_ROOT)) {
        printf("Failed at initializing PerfHound.\n");
        exit(1);
    }

    phmpi_set_tag(1, 0, M2S(STRCAT(KNAME, _Test)));
    phmpi_set_tag(1, 1, M2S(STRCAT(KNAME, _Start)));
    phmpi_set_tag(1, 2, M2S(STRCAT(KNAME, _End)));
    ph_example_set_events(mode);
    phmpi_commit();

    // Warm up
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    clock_gettime(CLOCK_MONOTONIC, &tv);
    sec = tv.tv_sec;
    nsec = tv.tv_nsec;
    nsec = sec * 1e9 + nsec + 1e9;

    while (tv.tv_sec * 1e9 + tv.tv_nsec < nsec) {
        clock_gettime(CLOCK_MONOTONIC, &tv);
        srand(tv.tv_nsec);
        res += rand() % 2 + tv.tv_nsec;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

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


    // Measure kernel
    while ((measure_counter++) < NMEASURE) {

        phmpi_read(1, 1, 0.0);


#pragma GCC unroll 10240
        for (int i = 0; i < NINS; ++i) {
            a[2] = a[0] * a[1];
            a[3] = a[0] + a[1];
            a[0] = a[2] * a[3];
            a[1] = a[2] + a[3];
            a[0] = a[3];
            a[1] = a[0];
            a[2] = a[1];
            a[3] = a[2];
        }

        phmpi_read(1, 2, 0.0);

    }

    asm volatile (
        "mov %%r11, %0 \n\t"
        : "=r"(res)
        :
        : "r11", "memory"
    );
    // printf("%d\n", res);
    MPI_Barrier(MPI_COMM_WORLD);

    phmpi_finalize();


    MPI_Finalize();
    return 0;
}
