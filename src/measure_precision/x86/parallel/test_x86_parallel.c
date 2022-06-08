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

#define DELAY_CDQ 1
#define KERNEL_CDQ \
    asm volatile ( \
        "cdq\n\t" \
        : \
        : \
        : "rax", "rdx", "memory" \
    );

#define DELAY_RDTSC 25
#define KERNEL_RDTSC \
    asm volatile ( \
        "rdtsc\n\t" \
        : \
        : \
        : "rax", "rdx", "memory" \
    );

#define DELAY_RDTSCP 32
#define KERNEL_RDTSCP \
    asm volatile ( \
        "rdtscp\n\t" \
        : \
        : \
        : "rax", "rcx", "rdx", "memory" \
    );

#define LFENCE 4
#define KERNEL_LFENCE \
    asm volatile ( \
        "lfence\n\t" \
        : \
        : \
        : "memory" \
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
        "add $1, %%rax \n\t" \
        : \
        : \
        : "rax", "memory" \
    );

#define DELAY_MUL 3
#define KERNEL_MUL \
    asm volatile ( \
        "imul $1, %%rax \n\t" \
        : \
        : \
        : "rax", "memory" \
    );

#define DELAY_LD 1
#define KERNEL_LD \
    asm volatile ( \
        "mov (%%rsp), %%r11 \n\t" \
        "mov (%%rsp), %%r11 \n\t" \
        : \
        : \
        : "r11", "memory" \
    );

#define DELAY_ST 1
#define KERNEL_ST \
    asm volatile ( \
        "mov %%r11, (%%rsp) \n\t" \
        : \
        : \
        : "r11", "memory" \
    );

#define KERNEL_LDST \
    asm volatile ( \
        "mov (%%rsp), %%r11 \n\t" \
        "mov %%r11, (%%rsp) \n\t" \
        : \
        : \
        : "r11", "memory" \
    );

#define KERNEL_FMA \
    asm volatile ( \
        "vfmadd213pd %%xmm0, %%xmm1, %%xmm1 \n\t" \
        "vfmadd213pd %%xmm1, %%xmm0, %%xmm0 \n\t" \
        : \
        : \
        : "xmm0", "xmm1", "memory" \
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
        "vpcmpeqw %%xmm0, %%xmm0, %%xmm0 \n\t"
        "vpsllq $54, %%xmm0, %%xmm0 \n\t"
        "vpsrlq $2, %%xmm0, %%xmm0 \n\t"
        "vmovaps %%xmm0, %%xmm1 \n\t"
        "movsd %%xmm0, %%xmm0 \n\t"
        "movsd %%xmm1, %%xmm1 \n\t"
        "mov (%%rsp), %%r11 \n\t"
        :
        :
        : "r11", "xmm0", "xmm1", "memory"
    );

    while ((measure_counter++) < NMEASURE) {
        pfhmpi_read(1, 1, 0);

#pragma GCC unroll 256
        for (int i = 0; i < major_counts; ++i) {
            KERNEL(KNAME)
        }
#pragma GCC unroll 256
        for (int i = 0; i < minor_counts; ++i) {
            KERNEL_CDQ
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
    sprintf(dirname, "./data/%s/nprocs_%d/arrlen_%d/%s_%s_test_6248/cydelay_%d", tool, nprocs, ARRLEN, mode, op, CYDELAY);
    if (pfhmpi_init(dirname)) { 
        printf("Failed at initailizing PerfHound.\n");
        exit(1);
    }
    pfhmpi_set_tag(1, 0, M2S(STRCAT(KNAME, _Test)));
    pfhmpi_set_tag(1, 1, M2S(STRCAT(KNAME, _Start)));
    pfhmpi_set_tag(1, 2, M2S(STRCAT(KNAME, _End)));

    if (strcmp(tool, "pfhd") == 0) {
        if (strcmp(mode, "EVX") == 0) {
            pfhmpi_set_evt("inst_retired.any_p");
            pfhmpi_set_evt("int_misc.recovery_cycles");
            // pfhmpi_set_evt("cpu_clk_unhalted.core_clk");
            pfhmpi_set_evt("uops_executed_port.port_0");
            pfhmpi_set_evt("uops_executed_port.port_1");
            pfhmpi_set_evt("uops_executed_port.port_2");
            pfhmpi_set_evt("uops_executed_port.port_3");
            pfhmpi_set_evt("uops_executed_port.port_5");
            pfhmpi_set_evt("uops_executed_port.port_6");
        } else if (strcmp(mode, "EV") == 0) {
            pfhmpi_set_evt("inst_retired.any_p");
            pfhmpi_set_evt("cpu_clk_unhalted.core_clk");
            pfhmpi_set_evt("int_misc.recovery_cycles");
            pfhmpi_set_evt("uops_executed_port.port_0");
        }
    } else {
        if (strcmp(mode, "EVX") == 0) {
            pfhmpi_set_evt("inst_retired.any_p");
            pfhmpi_set_evt("machine_clears.count");
            // pfhmpi_set_evt("cpu_clk_thread_unhalted.thread_p");
            pfhmpi_set_evt("uops_dispatched_port.port_0");
            pfhmpi_set_evt("uops_dispatched_port.port_1");
            pfhmpi_set_evt("uops_dispatched_port.port_2");
            pfhmpi_set_evt("uops_dispatched_port.port_3");
            pfhmpi_set_evt("uops_dispatched_port.port_5");
            pfhmpi_set_evt("uops_dispatched_port.port_6");
        } else if (strcmp(mode, "EV") == 0) {
            /*pfhmpi_set_evt("inst_retired.any_p");
            pfhmpi_set_evt("cpu_clk_thread_unhalted.thread_p");
            pfhmpi_set_evt("machine_clears.count");
            pfhmpi_set_evt("uops_issued.stall_cycles");*/
            pfhmpi_set_evt("cycle_activity.cycles_mem_any");
            pfhmpi_set_evt("mem_load_retired.l1_miss");
            pfhmpi_set_evt("mem_load_retired.l2_miss");
            pfhmpi_set_evt("mem_load_retired.l3_miss");
            // pfhmpi_set_evt("mem_load_l3_miss_retired.local_dram");
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
