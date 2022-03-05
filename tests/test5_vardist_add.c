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
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#ifdef PAPI
#include <papi.h>
#endif

#ifdef PERFHOUND
#include <pfhprobe.h>
#endif

// The number of tests(random number).
#ifndef NTESTS
#define NTESTS 2000
#endif

// Default cache size in bytes: 1048576 Bytes
#ifndef CACHEBYTES
#define CACHEBYTES 1048576
#endif

#define SCALAR 0.41421445

// Default kernel: ADD
#ifndef KNAME
#define KNAME ADD
#endif

// Default data collection mode: TS(Timestamp only)
#ifndef MODE
#define MODE TS
#endif

#define _M2S(x) #x
#define M2S(x) _M2S(x)
#define _STRCAT(x, y) x##y
#define STRCAT(x, y) _STRCAT(x, y)
#define KERNEL(x) STRCAT(DEP_, x)

// Predefined test kernels.
#ifdef __x86_64__

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

#elif __aarch64__

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

#endif

int main(int argc, char** argv) {
    int64_t res;
    struct timespec tv;
    char* op = M2S(KNAME);
    char* mode = M2S(MODE);
    uint64_t volatile sec, nsec; // For warmup
    int idx = 0, measure_counter = 0;

#ifdef PERFHOUND
    if (pfh_init("./data/test5_pfh_res_20220301T2002")) {
        printf("Failed at initailizing PerfHound.\n");
        exit(1);
    }

    pfh_set_tag(1, 0, M2S(STRCAT(KNAME, _Test)));
    pfh_set_tag(1, 1, M2S(STRCAT(KNAME, _Start)));
    pfh_set_tag(1, 2, M2S(STRCAT(KNAME, _End)));

    if (strcmp(mode, "EV") == 0) {
        pfh_set_evt("cpu_clk_unhalted.core_clk");
        pfh_set_evt("inst_retired.any_p");
        pfh_set_evt("uops_issued.any");
        pfh_set_evt("uops_retired.all");
    } else if (strcmp(mode, "EVX") == 0) {
        pfh_set_evt("cpu_clk_unhalted.core_clk");
        pfh_set_evt("inst_retired.any_p");
        pfh_set_evt("uops_issued.any");
        pfh_set_evt("uops_retired.all");
        pfh_set_evt("uops_executed_port.port_0");
        pfh_set_evt("uops_executed_port.port_1");
        pfh_set_evt("uops_executed_port.port_2");
        pfh_set_evt("uops_executed_port.port_3");
    }

    pfh_commit();
#endif

#ifdef PAPI
    int64_t register ns_1, ns_2;
    long long event_data[NTESTS * 2][8];
    long long tmp_before[8], tmp_after[8];
    int64_t nss[NTESTS * 2], cycs[NTESTS * 2];
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

    // Generating random number
    // Using urandom as the random source.
    int nins_list[NTESTS];
#ifdef GSL
    uint64_t seed;
    const gsl_rng_type *T;
    gsl_rng *r;
    FILE *fp_rand = NULL;
    
    fp_rand = fopen("/dev/urandom", "r");
    if (fp_rand == NULL) {
        printf("Failed to openrandom file.\n");
        return 1;
    }
    fread(&seed, sizeof(uint64_t), 1, fp_rand);
    fclose(fp_rand);
    
    gsl_rng_env_setup();
    T = gsl_rng_ranlux389;
    r = gsl_rng_alloc(T);
    gsl_rng_set(r, seed);

    fp_rand = fopen("test5_rand_list.txt", "w");
    if (fp_rand == NULL) {
        printf("Failed to write random number to file.\n");
        return 1;
    }
    for (int i = 0; i < NTESTS; i ++) {
        // A Gaussian distribution with mean=1000 and std=10.
        nins_list[i] = 1 + gsl_ran_gaussian(r, 10);
        // Lower bound and upper bound.
        nins_list[i] = nins_list[i] > 2? 2: nins_list[i];
        nins_list[i] = nins_list[i] < 0? 0: nins_list[i];
        fprintf(fp_rand, "%d,", nins_list[i]);
    }
    fclose(fp_rand);

#else
    FILE *fp_rand = NULL;
    fp_rand = fopen("test5_rand_list.txt", "r");
    if (fp_rand == NULL) {
        printf("Failed to read random number file test5_rand_list.txt.\n");
    }
    for (int i = 0; i < NTESTS; i ++) {
        fscanf(fp_rand, "%d,", &nins_list[i]);
    }
    fclose(fp_rand);
#endif

    // Array for cache flushing.
    double *a, *b, *c;
    int re;
    int flush_count = 0, flush_size = CACHEBYTES * 4;
    re = posix_memalign((void**)&a, 128, flush_size);
    if (re != 0) {
        printf("Failed to allocate memory for array a.\n");
        return 1;
    }
    re = posix_memalign((void**)&b, 128, flush_size);
    if (re != 0) {
        printf("Failed to allocate memory for array b.\n");
        return 1;
    }
    re = posix_memalign((void**)&c, 128, flush_size);
    if (re != 0) {
        printf("Failed to allocate memory for array c.\n");
        return 1;
    }
    flush_count = flush_size / 8; // 8 Bytes for each Double FP.
    for (int i = 0; i < flush_count; i ++) {
        a[i] = 2.0;
        b[i] = 1.0;
        c[i] = 0.0000001;
    }
    printf("Cache flush size: %d, with %d elements.\n", flush_size, flush_count);


    // Warm up
    clock_gettime(CLOCK_MONOTONIC, &tv);
    sec = tv.tv_sec;
    nsec = tv.tv_nsec;
    nsec = sec * 1e9 + nsec + 5e8;

    while (tv.tv_sec * 1e9 + tv.tv_nsec < nsec) {
        clock_gettime(CLOCK_MONOTONIC, &tv);
        srand(tv.tv_nsec);
        res += rand() % 2 + tv.tv_nsec;
    }

#ifdef __x86_64__
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

#elif __aarch64__
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

#endif
    // Measure kernel
    float perc_flag = 0.1;
    for (int iins = 0; iins < NTESTS; iins ++) {
        int nins = nins_list[iins];
        // Cache flushing
        for (int i = 0; i < flush_count; i ++) {
            a[i] = b[i] + SCALAR * c[i];
        }

        // Benchmarking
        // First timing
#ifdef PERFHOUND
        pfh_read(1, 1, 0);
#endif

#ifdef PAPI
        PAPI_read(EventSet, tmp_before);
        ns_1 = PAPI_get_real_nsec();
#endif
        // Computing kernel
#ifdef __x86_64__
        for (int i = nins; i > 0; i--) {
            asm volatile (
                KERNEL(KNAME)
                :
                :
                : "r10", "r11", "r12", "xmm0", "xmm1", "memory"
            );
        }
#elif __aarch64__

        for (int i = nins; i > 0; i--) {
            asm volatile (
                KERNEL(KNAME)
                :
                :
                : "x0", "x1", "x2", "x3", "v0", "v1", "memory"
            );
        }

#endif
        // Second timing.
#ifdef PERFHOUND
        pfh_read(1, 2, 0);
#endif

#ifdef PAPI
        PAPI_read(EventSet, tmp_after);
        ns_2 = PAPI_get_real_nsec();
        nss[idx] = ns_1;
        nss[idx+1] = ns_2;
        cycs[idx] = tmp_before[0];
        cycs[idx+1] = tmp_after[0];
        if (strcmp(mode, "EV") == 0) {
            memcpy(event_data[idx], tmp_before, 4 * sizeof(int64_t));
            memcpy(event_data[idx+1], tmp_after, 4 * sizeof(int64_t));
        } else if (strcmp(mode, "EVX") == 0) {
            memcpy(event_data[idx], tmp_before, 8 * sizeof(int64_t));
            memcpy(event_data[idx+1], tmp_after, 8 * sizeof(int64_t));
        }
        idx += 2;
#endif
        if (iins > NTESTS * perc_flag) {
            printf("Process: %.0f\%\n", perc_flag * 100);
            perc_flag += 0.1;
        }
    }

    // Moving the result to memory for preventing compiler from ignoring the register.
#ifdef __x86_64__

    asm volatile (
        "mov %%r11, %0 \n\t"
        : "=r"(res)
        :
        : "r11", "memory"
    );

#elif __aarch64__

    asm volatile (
        "mov x0, %0 \n\t"
        : "=r"(res)
        :
        : "x0", "memory"
    );

#endif
    // printf("%d\n", res);

#ifdef PERFHOUND
    pfh_finalize();
#endif

#ifdef PAPI
    for (int i = 0; i < 2 * NTESTS; i+=2) {
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
    return 0;
}
