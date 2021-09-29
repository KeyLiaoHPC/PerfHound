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

#ifdef PAPI
#include <papi.h>
#endif

#ifdef PERFHOUND
#include <pfh_mpi.h>
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

    MPI_Init(NULL, NULL);

#ifdef PERFHOUND
    char dirname[256];
    sprintf(dirname, "./PerfHound_res/%s_%s_test_6248_20210909/NINS=%d", mode, op, NINS);

    if (pfhmpi_init("../data/pfh_0929")) {
        printf("Failed at initailizing PerfHound.\n");
        exit(1);
    }

    pfhmpi_set_tag(1, 0, M2S(STRCAT(KNAME, _Test)));
    pfhmpi_set_tag(1, 1, M2S(STRCAT(KNAME, _Start)));
    pfhmpi_set_tag(1, 2, M2S(STRCAT(KNAME, _End)));

    if (strcmp(mode, "TS") == 0) {
        //pfhmpi_set_evt("cpu_clk_unhalted.core_clk");
        pfhmpi_set_evt("CPU_CLK_UNHALTED");
        pfhmpi_set_evt("inst_retired.any_p");
        pfhmpi_set_evt("uops_issued.any");
        pfhmpi_set_evt("uops_retired.all");
    } else if (strcmp(mode, "EVX") == 0) {
        pfhmpi_set_evt("cpu_clk_unhalted.core_clk");
        pfhmpi_set_evt("inst_retired.any_p");
        pfhmpi_set_evt("uops_issued.any");
        pfhmpi_set_evt("uops_retired.all");
        pfhmpi_set_evt("uops_executed_port.port_0");
        pfhmpi_set_evt("uops_executed_port.port_1");
        pfhmpi_set_evt("uops_executed_port.port_2");
        pfhmpi_set_evt("uops_executed_port.port_3");
    }

    pfhmpi_commit();
#endif

#ifdef PAPI
    int64_t register ns_1, ns_2;
    long long event_data[NMEASURE * 2][8];
    long long tmp_before[8], tmp_after[8];
    int64_t nss[NMEASURE * 2], cycs[NMEASURE * 2];
    char event_names[8][64] = {"CPU_CLK_THREAD_UNHALTED:THREAD_P", "INST_RETIRED:ANY_P",
                                "UOPS_ISSUED:ANY", "UOPS_RETIRED:ALL",
                                "UOPS_DISPATCHED_PORT:PORT_0", "UOPS_DISPATCHED_PORT:PORT_1",
                                "UOPS_DISPATCHED_PORT:PORT_2", "UOPS_DISPATCHED_PORT:PORT_3"};

    char fname[256];
    int cpu = sched_getcpu();
    sprintf(fname, "./papi_out_c%d.csv", cpu);
    FILE* fp = fopen(fname, "w");
    fprintf(fp, "cycle,nanosec");

    int EventSet = PAPI_NULL;
    PAPI_library_init(PAPI_VER_CURRENT);
    PAPI_create_eventset(&EventSet);

    if (strcmp(mode, "EV") == 0) {
        for (int i = 0; i < 4; ++i) {
            PAPI_add_named_event(EventSet, event_names[i]);
            fprintf(fp, ",%s", event_names[i]);
        }
    } else if (strcmp(mode, "EVX") == 0) {
        for (int i = 0; i < 8; ++i) {
            PAPI_add_named_event(EventSet, event_names[i]);
            fprintf(fp, ",%s", event_names[i]);
        }
    } else {
        PAPI_add_named_event(EventSet, event_names[0]);
    }
    fprintf(fp, "\n");
    PAPI_start(EventSet);
#endif

    // Warm up
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    clock_gettime(CLOCK_MONOTONIC, &tv);
    sec = tv.tv_sec;
    nsec = tv.tv_nsec;
    nsec = sec * 1e9 + nsec + 5e8;

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

#ifdef PERFHOUND
        pfhmpi_read(1, 1, 0);
#endif

#ifdef PAPI
        PAPI_read(EventSet, tmp_before);
        ns_1 = PAPI_get_real_nsec();
#endif

#pragma GCC unroll 10240
        for (int i = 0; i < NINS; ++i) {
            asm volatile (
                KERNEL(KNAME)
                :
                :
                : "r10", "r11", "r12", "xmm0", "xmm1", "memory"
            );
        }

#ifdef PERFHOUND
        pfhmpi_read(1, 2, 0);
#endif

#ifdef PAPI
        PAPI_read(EventSet, tmp_after);
        ns_2 = PAPI_get_real_nsec();
        nss[idx] = ns_1;
        nss[idx + 1] = ns_2;
        cycs[idx] = tmp_before[0];
        cycs[idx + 1] = tmp_after[0];
        if (strcmp(mode, "EV") == 0) {
            memcpy(event_data[idx], tmp_before, 4 * sizeof(int64_t));
            memcpy(event_data[idx + 1], tmp_after, 4 * sizeof(int64_t));
        } else if (strcmp(mode, "EVX") == 0) {
            memcpy(event_data[idx], tmp_before, 8 * sizeof(int64_t));
            memcpy(event_data[idx + 1], tmp_after, 8 * sizeof(int64_t));
        }
        idx += 2;
#endif
    }

    asm volatile (
        "mov %%r11, %0 \n\t"
        : "=r"(res)
        :
        : "r11", "memory"
    );
    // printf("%d\n", res);
    MPI_Barrier(MPI_COMM_WORLD);

#ifdef PERFHOUND
    pfhmpi_finalize();
#endif

#ifdef PAPI
    for (int i = 0; i < 2 * NMEASURE; i+=2) {
        fprintf(fp, "%lld,%lld", cycs[i+1] - cycs[i], nss[i+1] - nss[i]);
        if (strcmp(mode, "EV") == 0) {
            for (int j = 0; j < 4; ++j) {
                fprintf(fp, ",%lld", event_data[i+1][j] - event_data[i][j]);
            }
        } else if (strcmp(mode, "EVX") == 0) {
            for (int j = 0; j < 8; ++j) {
                fprintf(fp, ",%lld", event_data[i+1][j] - event_data[i][j]);
            }
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
    PAPI_shutdown();
#endif

    MPI_Finalize();
    return 0;
}
