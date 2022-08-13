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
#include <perfhound_mpi.h>

// Tool name
#ifndef TOOL
#define TOOL pfhd
#endif

// PerfHound mode
#ifndef MODE
#define MODE TS
#endif

// name of test kernel, default is ADD
#ifndef KNAME
#define KNAME ADD
#endif

// number of delayed cycles
#ifndef CYDELAY
#define CYDELAY 0
#endif

// used for flush cache
#ifndef ARRLEN
#define ARRLEN 3604480
#endif

// number of repeat run
#ifndef NMEASURE
#define NMEASURE 10000
#endif

#define _M2S(x) #x
#define M2S(x) _M2S(x)
#define _STRCAT(x, y) x##y
#define STRCAT(x, y) _STRCAT(x, y)
#define DELAY(x) STRCAT(DELAY_, x)
#define KERNEL(x) STRCAT(KERNEL_, x)

#define DELAY_ISB 22
#define KERNEL_ISB \
    asm volatile ( \
        "isb\n\t" \
        : \
        : \
        : "memory" \
    );

#define DELAY_MRS 2
#define KERNEL_MRS \
    asm volatile ( \
        "mrs x10, cntvct_el0\n\t" \
        : \
        : \
        : "x10", "memory" \
    );

#define DELAY_NOP 1
#define KERNEL_NOP \
    asm volatile ( \
        "nop\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        "nop\n\t" \
        : \
        : \
        : "memory" \
    );

#define DELAY_ADD 1
#define KERNEL_ADD \
    asm volatile ( \
        "add x10, x10, #1 \n\t" \
        : \
        : \
        : "x10", "memory" \
    );

#define DELAY_MUL 4
#define KERNEL_MUL \
    asm volatile ( \
        "mul x10, x10, #1 \n\t" \
        : \
        : \
        : "x10", "memory" \
    );

#define DELAY_LD 1
#define KERNEL_LD \
    asm volatile ( \
        "ldr x9, [sp] \n\t" \
        "ldr x9, [sp] \n\t" \
        : \
        : \
        : "x9", "memory" \
    );

#define DELAY_ST 1
#define KERNEL_ST \
    asm volatile ( \
        "str x9, [sp] \n\t" \
        : \
        : \
        : "x9", "memory" \
    );

#define KERNEL_LDST \
    asm volatile ( \
        "ldr x9, [sp] \n\t" \
        "str x9, [sp] \n\t" \
        : \
        : \
        : "x9", "memory" \
    );

#define KERNEL_FMA \
    asm volatile ( \
        "fmla v0.4s, v1.4s, v1.4s \n\t" \
        : \
        : \
        : "v0", "v1", "memory" \
    );

void flush_cache(double* arr1, double* arr2, double scalar) {
    for (int i = 0; i < ARRLEN; i += 8) {
        arr1[i] += scalar * arr2[i];
    }
}

void one_round_test(double* arr1, double* arr2, double scalar) {
    int measure_counter = 0;
    int tot_delay = CYDELAY;
    int major_delay = DELAY(KNAME);
    int major_counts = (int)(tot_delay / major_delay);
    int minor_counts = (tot_delay - major_delay * major_counts);

    // register initialization
    asm volatile (
        "fmov v0.4s, #1.00000000 \n\t"
        "fmov v1.4s, #2.00000000 \n\t"
        "ldr x9, [sp] \n\t"
        :
        :
        : "x9", "v0", "v1", "memory"
    );

    while ((measure_counter++) < NMEASURE) {
        pfhmpi_read(1, 1, 0);

#pragma GCC unroll 256
        for (int i = 0; i < major_counts; ++i) {
            KERNEL(KNAME)
        }
#pragma GCC unroll 256
        for (int i = 0; i < minor_counts; ++i) {
            KERNEL_ADD
        }

        pfhmpi_read(1, 2, 0);

#if ARRLEN
        flush_cache(arr1, arr2, scalar);
#endif
    }
}

int main(int argc, char** argv) {
    int nprocs;
    char dirname[256];
    char* op = M2S(KNAME);
    char* mode = M2S(MODE);
    char* tool = M2S(TOOL);

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    // create directory
    sprintf(dirname, "./data/%s/nprocs_%d/arrlen_%d/%s_%s_test_kp920/cydelay_%d", tool, nprocs, ARRLEN, mode, op, CYDELAY);
    if (pfhmpi_init(dirname)) {
        printf("Failed at initailizing PerfHound.\n");
        exit(1);
    }
    pfhmpi_set_tag(1, 0, M2S(STRCAT(KNAME, _Test)));
    pfhmpi_set_tag(1, 1, M2S(STRCAT(KNAME, _Start)));
    pfhmpi_set_tag(1, 2, M2S(STRCAT(KNAME, _End)));

    if (strcmp(tool, "pfhd") == 0) {
        if (strcmp(mode, "EVX") == 0) {
            pfhmpi_set_evt("CPU_CYCLES");
            pfhmpi_set_evt("INST_RETIRED");
            pfhmpi_set_evt("STALL_FRONTEND");
            pfhmpi_set_evt("STALL_BACKEND");
            pfhmpi_set_evt("L1D_TLB");
            pfhmpi_set_evt("L1D_CACHE");
            pfhmpi_set_evt("L1I_CACHE");
            pfhmpi_set_evt("L2D_CACHE");
            pfhmpi_set_evt("BR_RETIRED");
            pfhmpi_set_evt("MEM_ACCESS");
            pfhmpi_set_evt("L1D_TLB_REFILL");
            pfhmpi_set_evt("L1D_CACHE_REFILL");
        } else if (strcmp(mode, "EV") == 0) {
            pfhmpi_set_evt("CPU_CYCLES");
            pfhmpi_set_evt("INST_RETIRED");
            pfhmpi_set_evt("STALL_FRONTEND");
            pfhmpi_set_evt("STALL_BACKEND");
        }
    } else {
        if (strcmp(mode, "EVX") == 0) {
            pfhmpi_set_evt("CPU_CYCLES");
            pfhmpi_set_evt("INST_RETIRED");
            pfhmpi_set_evt("STALL_FRONTEND");
            pfhmpi_set_evt("STALL_BACKEND");
            pfhmpi_set_evt("L1D_TLB");
            pfhmpi_set_evt("L1D_CACHE");
            pfhmpi_set_evt("L1I_CACHE");
            pfhmpi_set_evt("L2D_CACHE");
            pfhmpi_set_evt("BR_RETIRED");
            pfhmpi_set_evt("MEM_ACCESS");
            pfhmpi_set_evt("L1D_TLB_REFILL");
            pfhmpi_set_evt("L1D_CACHE_REFILL");
        } else if (strcmp(mode, "EV") == 0) {
            pfhmpi_set_evt("CPU_CYCLES");
            pfhmpi_set_evt("INST_RETIRED");
            pfhmpi_set_evt("STALL_FRONTEND");
            pfhmpi_set_evt("STALL_BACKEND");
        }
    }
    pfhmpi_commit();

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    // initial array
    double scalar;
    struct timespec tv;
    clock_gettime(CLOCK_MONOTONIC, &tv);
    double* arr1 = (double*) malloc(ARRLEN * sizeof(double));
    double* arr2 = (double*) malloc(ARRLEN * sizeof(double));
    // srand(0);
    srand(tv.tv_nsec);
    scalar = ((rand() * 1.0) / RAND_MAX);
    for (int i = 0; i < ARRLEN; ++i) {
        arr1[i] = ((rand() * 1.0) / RAND_MAX);
        arr2[i] = ((rand() * 1.0) / RAND_MAX);
    }
    flush_cache(arr1, arr2, scalar);
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    one_round_test(arr1, arr2, scalar);
    /*asm volatile (
        "mov %%r11, %0 \n\t"
        : "=r"(res)
        :
        : "r11", "memory"
    );
    printf("%d\n", res);*/

    MPI_Barrier(MPI_COMM_WORLD);
    free(arr1);
    free(arr2);
    pfhmpi_finalize();
    MPI_Finalize();
    return 0;
}
