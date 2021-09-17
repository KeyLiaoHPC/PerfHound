#define _GNU_SOURCE
#define _ISOC11_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdarg.h>
#include <unistd.h>
#include <sched.h>



#ifdef PFH
#include "pfhprobe.h"
#endif

#ifdef PAPI
#include "papi.h"
#endif

#ifndef NADD
#define NADD 10
#endif

#ifndef NMEASURE
#define NMEASURE 100
#endif

#define DEP_ADD "lea    (%%r11,%%r13), %%r12" "\n\t" \
                "lea    (%%r12,%%r13), %%r11" "\n\t" 

#define DEP_ADD_X1 "lea    (%%r11,%%r13), %%r12" "\n\t" \
                   "lea    (%%r12,%%r13), %%r11" "\n\t" 

#define DEP_ADD_X2 DEP_ADD_X1 DEP_ADD_X1


int
main(int argc, char **argv) {
    //int register r0 = 0, r1 = 1, r2 = 2;

    int i, ic;
    int64_t res;
    struct timespec tv;
    uint64_t volatile sec, nsec; // For warmup

#ifdef PFH
    /*=== Initialize PerfHound ===*/
    if (pfh_init("./pfh_20210910")) {
        printf("Failed at initailizing PerfHound.\n");
        exit(1);
    }

    pfh_set_tag(1, 0, "ADD_Test");
    pfh_set_tag(1, 1, "ADD_Start");
    pfh_set_tag(1, 2, "ADD_End");

#ifdef __x86_64__
    pfh_set_evt("inst_retired.any_p");
    pfh_set_evt("uops_issued.any");
    pfh_set_evt("uops_retired.all");
    pfh_set_evt("icache.misses");
    pfh_set_evt("uops_executed_port.port_0");
    pfh_set_evt("uops_executed_port.port_1");
    pfh_set_evt("uops_executed_port.port_2");
    pfh_set_evt("uops_executed_port.port_3");
#endif
    
#ifdef __aarch64__
    pfh_set_evt("L1D_CACHE");
    pfh_set_evt("LD_RETIRED");
    pfh_set_evt("ST_RETIRED");
    pfh_set_evt("INST_RETIRED");
    pfh_set_evt("L1I_CACHE");
    pfh_set_evt("BR_RETIRED");
    pfh_set_evt("STALL_FRONTEND");
    pfh_set_evt("STALL_BACKEND");
#endif

    pfh_commit();
#endif

#ifdef PAPI
    long long cyc_1, cyc_2;
    int64_t register us_1, us_2;
    int64_t cyc[NMEASURE*2], us[NMEASURE*2];
    long long papi_evt[NMEASURE*2][8];
    int EventSet = PAPI_NULL;

    if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT ) {
       printf("PAPI initialization error! \n");
       exit(1);
    }
    PAPI_create_eventset(&EventSet);
    //PAPI_add_event(EventSet, PAPI_TOT_INS)
    //PAPI_add_event(EventSet, PAPI_TOT_CYC);
    char event_names[8][64] =  {"CPU_CLK_UNHALTED.THREAD", "INST_RETIRED:ANY_P",
                                "UOPS_ISSUED:ANY", "UOPS_RETIRED:ALL",
                                "UOPS_DISPATCHED_PORT:PORT_0", "UOPS_DISPATCHED_PORT:PORT_1",
                                "UOPS_DISPATCHED_PORT:PORT_2", "UOPS_DISPATCHED_PORT:PORT_3"};
    for (int i = 0; i < 8; ++i) {
        PAPI_add_named_event(EventSet, event_names[i]);
    }

    PAPI_start(EventSet);
#endif

    /*=== Warm up ===*/
    clock_gettime(CLOCK_MONOTONIC, &tv);
    sec = tv.tv_sec;
    nsec = tv.tv_nsec;
    nsec = sec * 1e9 + nsec + 5e8;
    while(tv.tv_sec * 1e9 + tv.tv_nsec < nsec) {
        clock_gettime(CLOCK_MONOTONIC, &tv);
        srand(tv.tv_nsec);
        res += rand() % 2 + tv.tv_nsec;
    }

    asm volatile (
            "mov    $0, %%r11" "\n\t"
            "mov    $0, %%r12" "\n\t"
            "mov    $1, %%r13" "\n\t"
            :
            :
            : "r11", "r12", "r13"
            );

    ic = NMEASURE;

    /*=== Measured kernel ===*/
    while (ic--){
        i = 2 * (NMEASURE - ic - 1);

#ifdef PFH
        pfh_read(1, 1, 0);
#endif

#ifdef PAPI
        PAPI_read(EventSet, papi_evt[i]);
        us_1 = PAPI_get_real_nsec();
#endif

#pragma GCC unroll 1024
        for (int kk = 0; kk < NADD; kk ++) {
            asm volatile (
                DEP_ADD 
                :
                :
                : "r11", "r12", "r13", "memory"
            );
        }
#ifdef PFH
        pfh_read(1, 2, 0);
#endif

#ifdef PAPI
        /*=== Dump PAPI result to array.  ===*/
        PAPI_read(EventSet, papi_evt[i+1]);
        us_2 = PAPI_get_real_nsec();

        cyc[i] = papi_evt[i][0];
        cyc[i+1] = papi_evt[i+1][0];
        us[i] = us_1;
        us[i+1] = us_2;
#endif
    }

    asm volatile (
            "mov    %%r11, %0" "\n\t"
            : "=r"(res)
            :
            : "r11", "r12", "r13", "memory"
            );

    printf("%d\n", res);

#ifdef PFH
    pfh_finalize();
#endif
    
#ifdef PAPI
    /*=== Dump PAPI result to file.  ===*/
    FILE *fp;
    char fname[1024];
    int cpu = sched_getcpu();
    sprintf(fname, "./papi_out_c%d.csv", cpu);
    fp = fopen(fname, "w");
    for (i = 0; i < NMEASURE*2; i ++) {
        fprintf(fp, "%lld, %lld", cyc[i], us[i]);
	    for (int j = 0; j < 8; j ++) {
            fprintf(fp, ", %lld", papi_evt[i][j]);
	    }
	    fprintf(fp, "\n");
    }
    fclose(fp);
    PAPI_shutdown();
#endif

    return 0;
}
