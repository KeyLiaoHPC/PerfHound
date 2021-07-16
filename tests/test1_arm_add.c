#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

#ifdef PERFHOUND
#include "varapi.h"
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

#define DEP_ADD "add    x3, x4, x5" "\n\t" \
                "add    x4, x3, x5" "\n\t" 

#define DEP_ADD_X1 "add    x3, x4, x5" "\n\t" \
                   "add    x4, x3, x5" "\n\t" 

#define DEP_ADD_X2 DEP_ADD_X1 DEP_ADD_X1



int
main(int argc, char **argv) {
    //int register r0 = 0, r1 = 1, r2 = 2;

    int i, ic;
    int64_t res;
    struct timespec tv;
    uint64_t volatile sec, nsec; // For warmup

#ifdef PERFHOUND
    /*=== Initialize PerfHound ===*/
    if (vt_init(".", "ipdps-1_6248")) {
        printf("Failed at initailizing PerfHound.\n");
        exit(1);
    }

    vt_commit();
#endif

#ifdef PAPI
    long long cyc_1, cyc_2;
    int64_t register us_1, us_2;
    int64_t cyc[NMEASURE*2], us[NMEASURE*2];
    int EventSet = PAPI_NULL;

    if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT ) {
       printf("PAPI initialization error! \n");
       exit(1);
    }
    PAPI_create_eventset(&EventSet);
    //PAPI_add_event(EventSet, PAPI_TOT_INS)
    PAPI_add_event(EventSet, PAPI_TOT_CYC);

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
            "mov    x3, #0" "\n\t"
            "mov    x4, #0" "\n\t"
            "mov    x5, #1" "\n\t"
            :
            :
            : "x3", "x4", "x5"
            );

    ic = NMEASURE;

    /*=== Measured kernel ===*/
    while (ic--){

#ifdef PERFHOUND
        vt_read(1, 1, 0, 0, 0);
#endif

#ifdef PAPI
        PAPI_read(EventSet, &cyc_1);
        us_1 = PAPI_get_real_nsec();
#endif

#pragma GCC unroll 1024
        for (i = 0; i < NADD; i ++) {
             asm volatile(
                    DEP_ADD 
                    :
                    :
                    : "x3", "x4", "x5", "memory"
                    );
        }
#ifdef PERFHOUND
        vt_read(1, 2, 0, 0, 0);
#endif

#ifdef PAPI
        /*=== Dump PAPI result to array.  ===*/
        PAPI_read(EventSet, &cyc_2);
        us_2 = PAPI_get_real_nsec();

        i = 2 * (NMEASURE - ic - 1);
        cyc[i] = cyc_1;
        cyc[i+1] = cyc_2;
        us[i] = us_1;
        us[i+1] = us_2;
#endif
    }

    asm volatile (
            "mov    %0, x4" "\n\t"
            : "=r"(res)
            :
            : "r11", "r12", "r13", "memory"
            );

    printf("%d\n", res);

#ifdef PERFHOUND
    vt_clean();
#endif
    
#ifdef PAPI
    /*=== Dump PAPI result to file.  ===*/
    FILE *fp = fopen("./papi_out.txt", "w");
    for (i = 0; i < NMEASURE*2; i ++) {
        fprintf(fp, "%lld, %lld\n", cyc[i], us[i]);
    }
    fclose(fp);
    PAPI_shutdown();
#endif

    return 0;
}
