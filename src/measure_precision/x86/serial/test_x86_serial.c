#define _GNU_SOURCE
#define _ISOC11_SOURCE

#include <time.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <perfhound.h>

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

#define DELAY_LFENCE 4
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
        pfh_read(1, 1, 0);

#pragma GCC unroll 256
        for (int i = 0; i < major_counts; ++i) {
            KERNEL(KNAME)
        }
#pragma GCC unroll 256
        for (int i = 0; i < minor_counts; ++i) {
            KERNEL_CDQ
        }

        pfh_read(1, 2, 0);

#if ARRLEN
        flush_cache(arr1, arr2, scalar);
#endif
    }
}

int main(int argc, char** argv) {
    char dirname[256];
    char* op = M2S(KNAME);
    char* mode = M2S(MODE);
    char* tool = M2S(TOOL);

    // create directory
    sprintf(dirname, "./data/%s/arrlen_%d/%s_%s_test_6248/cydelay_%d", tool, ARRLEN, mode, op, CYDELAY);
    if (pfh_init(dirname)) {
        printf("Failed at initailizing PerfHound.\n");
        exit(1);
    }
    pfh_set_tag(1, 0, M2S(STRCAT(KNAME, _Test)));
    pfh_set_tag(1, 1, M2S(STRCAT(KNAME, _Start)));
    pfh_set_tag(1, 2, M2S(STRCAT(KNAME, _End)));

    if (strcmp(tool, "pfhd") == 0) {
        if (strcmp(mode, "EVX") == 0) {
            pfh_set_evt("inst_retired.any_p");
            pfh_set_evt("int_misc.recovery_cycles");
            pfh_set_evt("uops_executed_port.port_0");
            pfh_set_evt("uops_executed_port.port_1");
            pfh_set_evt("uops_executed_port.port_2");
            pfh_set_evt("uops_executed_port.port_3");
            pfh_set_evt("uops_executed_port.port_5");
            pfh_set_evt("uops_executed_port.port_6");
        } else if (strcmp(mode, "EV") == 0) {
            pfh_set_evt("icache.misses");
            pfh_set_evt("inst_retired.any_p");
            pfh_set_evt("int_misc.recovery_cycles");
            pfh_set_evt("cpu_clk_unhalted.core_clk");
        }
    } else {
        if (strcmp(mode, "EVX") == 0) {
            pfh_set_evt("inst_retired.any_p");
            pfh_set_evt("machine_clears.count");
            pfh_set_evt("icache_64b.iftag_miss");
            pfh_set_evt("mem_load_retired.l3_miss");
            pfh_set_evt("frontend_retired.l1i_miss");
            pfh_set_evt("cycle_activity.stalls_total");
            pfh_set_evt("cycle_activity.cycles_mem_any");
            pfh_set_evt("cycle_activity.stalls_mem_any");
        } else if (strcmp(mode, "EV") == 0) {
            pfh_set_evt("inst_retired.any_p");
            pfh_set_evt("icache_64b.iftag_miss");
            pfh_set_evt("mem_load_retired.l3_miss");
            pfh_set_evt("cycle_activity.cycles_mem_any");
            // pfh_set_evt("uops_issued.stall_cycles");
            // pfh_set_evt("cpu_clk_thread_unhalted.thread_p");
            // pfh_set_evt("mem_load_l3_miss_retired.local_dram");
        }
    }
    pfh_commit();

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

    one_round_test(arr1, arr2, scalar);
    /*asm volatile (
        "mov %%r11, %0 \n\t"
        : "=r"(res)
        :
        : "r11", "memory"
    );
    printf("%d\n", res);*/

    free(arr1);
    free(arr2);
    pfh_finalize();
    return 0;
}
