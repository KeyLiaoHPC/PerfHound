#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
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

#define DEP_ADD "add x0, x1, x2 \n\t" \
                "add x1, x2, x0 \n\t"

#define DEP_MUL "mul x0, x1, x2 \n\t" \
                "mul x1, x2, x0 \n\t"

#define DEP_LDST "ldr x3, [sp] \n\t" \
                 "str x3, [sp] \n\t" 

#define DEP_FMA "fmla v0.4s, v1.4s, v1.4s \n\t" \
                "fmla v0.4s, v1.4s, v1.4s \n\t"

#define DEP_LD "ldr x3, [sp] \n\t" \
               "ldr x3, [sp] \n\t" 

#define DEP_ST "str x3, [sp] \n\t" \
               "str x3, [sp] \n\t" 

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
    sprintf(dirname, "./PerfHound_res/%s_%s_test_920_20210908/NINS=%d", mode, op, NINS);

    if (pfhmpi_init("../data/pfhmpi_0918")) {
        printf("Failed at initailizing PerfHound.\n");
        exit(1);
    }

    pfhmpi_set_tag(1, 0, M2S(STRCAT(KNAME, _Test)));
    pfhmpi_set_tag(1, 1, M2S(STRCAT(KNAME, _Start)));
    pfhmpi_set_tag(1, 2, M2S(STRCAT(KNAME, _End)));

    if (strcmp(mode, "EV") == 0) {
        pfhmpi_set_evt("CPU_CYCLES");
        pfhmpi_set_evt("INST_RETIRED");
        pfhmpi_set_evt("BR_RETIRED");
        pfhmpi_set_evt("L1D_CACHE");
    } else if (strcmp(mode, "EVX") == 0) {
        pfhmpi_set_evt("CPU_CYCLES");
        pfhmpi_set_evt("INST_RETIRED");
        pfhmpi_set_evt("BR_RETIRED");
        pfhmpi_set_evt("L1D_CACHE");
        pfhmpi_set_evt("L1I_CACHE");
        pfhmpi_set_evt("L2D_CACHE");
        pfhmpi_set_evt("L1D_CACHE_REFILL");
        pfhmpi_set_evt("L1D_TLB");
        pfhmpi_set_evt("L1D_TLB_REFILL");
        pfhmpi_set_evt("MEM_ACCESS");
        pfhmpi_set_evt("STALL_FRONTEND");
        pfhmpi_set_evt("STALL_BACKEND");
    }

    pfhmpi_commit();
#endif

#ifdef PAPI
    int64_t register ns_1, ns_2;
    long long event_data[NMEASURE * 2][12];
    long long tmp_before[12], tmp_after[12];
    int64_t nss[NMEASURE * 2], cycs[NMEASURE * 2];
    char event_names[12][64] = {"CPU_CYCLES", "INST_RETIRED", "BR_RETIRED", "L1D_CACHE",
                                "L1I_CACHE", "L2D_CACHE", "L1D_CACHE_REFILL", "L1D_TLB",
                                "L1D_TLB_REFILL", "MEM_ACCESS", "STALL_FRONTEND", "STALL_BACKEND"};

    char filepath[256];
    sprintf(filepath, "./PAPI_res/%s_%s_test_920_20210908/NINS=%d/run.csv", mode, op, NINS);
    FILE* fp = fopen(filepath, "w");
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
        for (int i = 0; i < 12; ++i) {
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
        "fmov v0.4s, #1.00000000 \n\t"
        "fmov v1.4s, #2.00000000 \n\t"
        "mov x0, #0 \n\t"
        "mov x1, #1 \n\t"
        "mov x2, #2 \n\t"
        "ldr x3, [sp] \n\t"
        :
        :
        : "x0", "x1", "x2", "x3", "v0", "v1"
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
                : "x0", "x1", "x2", "x3", "v0", "v1", "memory"
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
            memcpy(event_data[idx], tmp_before, 12 * sizeof(int64_t));
            memcpy(event_data[idx + 1], tmp_after, 12 * sizeof(int64_t));
        }
        idx += 2;
#endif
    }

    asm volatile (
        "mov x0, %0 \n\t"
        : "=r"(res)
        :
        : "x0", "memory"
    );
    // printf("%d\n", res);

#ifdef PERFHOUND
    pfhmpi_finalize();
#endif

#ifdef PAPI
    for (int i = 0; i < 2 * NMEASURE; ++i) {
        fprintf(fp, "%lld,%lld", cycs[i], nss[i]);
        if (strcmp(mode, "EV") == 0) {
            for (int j = 0; j < 4; ++j) {
                fprintf(fp, ",%lld", event_data[i][j]);
            }
        } else if (strcmp(mode, "EVX") == 0) {
            for (int j = 0; j < 12; ++j) {
                fprintf(fp, ",%lld", event_data[i][j]);
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
