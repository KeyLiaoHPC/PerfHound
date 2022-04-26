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
#include <pfh_mpi.h>

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

    char dirname[256];
    sprintf(dirname, "./PerfHound_res/%s_%s_test_6248_20210909/NINS=%d", mode, op, NINS);

#ifdef PAPI
    if (pfhmpi_init("../data/pfh_papi")) {
#else
    if (pfhmpi_init("../data/pfh")) {
#endif
        printf("Failed at initailizing PerfHound.\n");
        exit(1);
    }

    pfhmpi_set_tag(1, 0, M2S(STRCAT(KNAME, _Test)));
    pfhmpi_set_tag(1, 1, M2S(STRCAT(KNAME, _Start)));
    pfhmpi_set_tag(1, 2, M2S(STRCAT(KNAME, _End)));

    if (strcmp(mode, "TS") == 0) {
        //pfhmpi_set_evt("cpu_clk_unhalted.core_clk");
        //pfhmpi_set_evt("CPU_CLK_UNHALTED");
        pfhmpi_set_evt("inst_retired.any_p");
        //pfhmpi_set_evt("uops_issued.any");
        //pfhmpi_set_evt("uops_retired.all");
    } else if (strcmp(mode, "EVX") == 0) {
#ifdef PAPI
        pfhmpi_set_evt("INST_RETIRED:ANY_P");
        pfhmpi_set_evt("UOPS_DISPATCHED_PORT:PORT_0");
        pfhmpi_set_evt("UOPS_DISPATCHED_PORT:PORT_1");
        pfhmpi_set_evt("UOPS_DISPATCHED_PORT:PORT_2");
        pfhmpi_set_evt("UOPS_DISPATCHED_PORT:PORT_4");
        pfhmpi_set_evt("UOPS_DISPATCHED_PORT:PORT_5");
        pfhmpi_set_evt("UOPS_DISPATCHED_PORT:PORT_6");
#else
        pfhmpi_set_evt("inst_retired.any_p");
        pfhmpi_set_evt("uops_executed_port.port_0");
        pfhmpi_set_evt("uops_executed_port.port_1");
        pfhmpi_set_evt("uops_executed_port.port_2");
        pfhmpi_set_evt("uops_executed_port.port_4");
        pfhmpi_set_evt("uops_executed_port.port_5");
        pfhmpi_set_evt("uops_executed_port.port_6");
#endif
    }

    pfhmpi_commit();

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

        pfhmpi_read(1, 1, 0);


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

        pfhmpi_read(1, 2, 0);

    }

    asm volatile (
        "mov %%r11, %0 \n\t"
        : "=r"(res)
        :
        : "r11", "memory"
    );
    // printf("%d\n", res);
    MPI_Barrier(MPI_COMM_WORLD);

    pfhmpi_finalize();


    MPI_Finalize();
    return 0;
}
