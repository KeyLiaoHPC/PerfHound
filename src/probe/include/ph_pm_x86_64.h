/*
 * =================================================================================
 * PerfHound - Detecting and analyzing performance variation in parallel program
 * 
 * Copyright (C) 2020 Key Liao (Liao Qiucheng)
 * 
 * This program is free software: you can redistribute it and/or modify it under the
 *  terms of the GNU General Public License as published by the Free Software 
 * Foundation, either version 3 of the License, or (at your option) any later 
 * version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with 
 * this program. If not, see https://www.gnu.org/licenses/.
 * 
 * =================================================================================
 * rec_ts_x86_64.h
 * Description: Macros for timer on x86_64.
 * Author: Key Liao
 * Modified: Jul. 2nd, 2020
 * Email: keyliaohpc@gmail.com
 * =================================================================================
 */
//==================================================================================

/* Comment Syntax */
/*
 * Long block document.
 */
/* Beginning remark for functioning block/procedure. */
// Short comment for variables, short explanation.

//==================================================================================

/* ph_enable_pmu sysfs is only for PMC config; hot-path reads still use rdpmc */
#include "ph_pmu_x86.h"

#define _X86_TSC_GHZ 2.49414

#define _pfc_read(rcx)                \
    "\n\tmov      $"#rcx", %%rcx                 "  \
    "\n\trdpmc                                   "  \
    "\n\tshl      $32,     %%rdx                 "  \
    "\n\tor       %%rax,   %%rdx                 "  \
    "\n\tmov      %%rdx,   %0                    "  \
    "\n\t"

/* Fixed-counter init: indices 0-2 are fixed; cfg=2 enables counting in OS+user mode */
#define _ph_init_ts                                                \
    do {                                                            \
        PH_PMU_CFG ph_cfgs[7] = {2, 2, 2, 0, 0, 0, 0};              \
        const PH_PMU_CNT ph_zero_cnts[7] = {0, 0, 0, 0, 0, 0, 0};   \
        if (ph_pmu_init() != 0) {                                   \
            printf("*** [PH-Probe] Failed to open ph_enable_pmu sysfs.\n"); \
            fflush(stdout);                                         \
            exit(1);                                                \
        }                                                           \
        ph_pmu_wr_cfgs(0, 7, ph_cfgs);                              \
        ph_pmu_wr_cnts(0, 7, ph_zero_cnts);                         \
    } while(0);                            

#define _ph_fini_ts     \
    ph_pmu_fini()

#define _ph_init_cy

#define _ph_reg_save                                   \
    asm volatile(                                       \
            "\n\tmov %%rax, %0"                         \
            "\n\tmov %%rcx, %1"                         \
            "\n\tmov %%rdx, %2"                         \
            "\n\t"                                      \
            :"=r" (r1), "=r" (r2), "=r"(r3)             \
            :                                           \
            : );                                        

#define _ph_reg_restore                                \
    asm volatile(                                       \
            "\n\tmov %0, %%rax"                         \
            "\n\tmov %1, %%rcx"                         \
            "\n\tmov %2, %%rdx"                         \
            "\n\t"                                      \
            :                                           \
            :"r" (r1), "r" (r2), "r"(r3)                \
            : );                                        


/* Read actual clock cycle from CPU_CLK_UNHALTED.THREAD */
#define _ph_read_cy(_cy)                               \
    do {                                                \
        register uint64_t cy;                           \
        asm volatile(                                   \
            _pfc_read(0x40000001)                       \
            : "=r"(cy)                                  \
            :                                           \
            : "memory", "rax", "rcx", "rdx"             \
        );                                              \
        _cy = cy;                                       \
    } while(0)


/* Read virtual timer */
#ifdef USE_SYSCALL_TS
#include <sys/syscall.h>
#define _ph_read_ns(_ns)   struct timespec ts;                                 \
                            syscall(__NR_clock_gettime, CLOCK_REALTIME, &ts);   \
                            (_ns) = ts.tv_sec * 1e9 + ts.tv_nsec;
                            
#else
#define _ph_read_ns(_ns)                               \
    do {                                                \
        register uint64_t ns;                           \
        asm volatile(                                   \
            "\n\tRDTSCP"                                \
            "\n\tshl $32, %%rdx"                        \
            "\n\tor  %%rax, %%rdx"                      \
            "\n\tmov %%rdx, %0"                         \
            "\n\t"                                      \
            :"=r" (ns)                                  \
            :                                           \
            : "memory", "%rax", "%rdx");                \
        _ns = ns;                                       \
    } while(0);

#endif

/* Macros for reading x86_64 events */
/* Parsing string events to hex event code */
#define _ph_parse_event(_code, _evstr)  \
    _code = ph_pmu_parse_cfg(_evstr);        \
    if(_code == 0) _code = 0xFFFFFFFF;

/* Set IA32_PERFEVTSELx */
#define _ph_config_event(_code_arr, _nevt)  \
    if ((_nevt)) {                                                  \
        do {                                                        \
            const PH_PMU_CNT ph_zeros[8] = {0, 0, 0, 0, 0, 0, 0, 0}; \
            ph_pmu_wr_cfgs(3, _nevt, _code_arr);                      \
            ph_pmu_wr_cnts(3, _nevt, ph_zeros);                       \
        } while(0);                                                 \
    }


/* Read PMC */
#define _pm_read(rcx, off)                      \
    "\n\tmov      $"#rcx", %%rcx            "   \
    "\n\trdpmc                              "   \
    "\n\tshl      $32,     %%rdx            "   \
    "\n\tor       %%rax,   %%rdx            "   \
    "\n\tmov      %%rdx,   "#off"(%0)       "   \
    "\n\t"

#define _ph_read_pm_1(arr)                     \
        asm volatile(                           \
            _pm_read(0x00000000, 0)             \
            :                                   \
            : "r"((arr))                        \
            : "memory", "%rax", "%rcx", "%rdx"  \
        );                                              

#define _ph_read_pm_2(arr)                      \
        asm volatile(                           \
            _pm_read(0x00000000, 0)             \
            _pm_read(0x00000001, 8)             \
            :                                   \
            : "r"((arr))                        \
            : "memory", "%rax", "%rcx", "%rdx"  \
        );                                       
        
#define _ph_read_pm_3(arr)                      \
        asm volatile(                           \
            _pm_read(0x00000000, 0)             \
            _pm_read(0x00000001, 8)             \
            _pm_read(0x00000002, 16)            \
            :                                   \
            : "r"((arr))                        \
            : "memory", "%rax", "%rcx", "%rdx"  \
        );                                       

#define _ph_read_pm_4(arr)                      \
        asm volatile(                           \
            _pm_read(0x00000000, 0)             \
            _pm_read(0x00000001, 8)             \
            _pm_read(0x00000002, 16)            \
            _pm_read(0x00000003, 24)            \
            :                                   \
            : "r"((arr))                        \
            : "memory", "%rax", "%rcx", "%rdx"  \
        );                                              

#define _ph_read_pm_5(arr)                      \
        asm volatile(                           \
            _pm_read(0x00000000, 0)             \
            _pm_read(0x00000001, 8)             \
            _pm_read(0x00000002, 16)            \
            _pm_read(0x00000003, 24)            \
            _pm_read(0x00000004, 32)            \
            :                                   \
            : "r"((arr))                        \
            : "memory", "%rax", "%rcx", "%rdx"  \
        );                                       


#define _ph_read_pm_6(arr)                      \
        asm volatile(                           \
            _pm_read(0x00000000, 0)             \
            _pm_read(0x00000001, 8)             \
            _pm_read(0x00000002, 16)            \
            _pm_read(0x00000003, 24)            \
            _pm_read(0x00000004, 32)            \
            _pm_read(0x00000005, 40)            \
            :                                   \
            : "r"((arr))                        \
            : "memory", "%rax", "%rcx", "%rdx"  \
        );                                       

#define _ph_read_pm_7(arr)                      \
        asm volatile(                           \
            _pm_read(0x00000000, 0)             \
            _pm_read(0x00000001, 8)             \
            _pm_read(0x00000002, 16)            \
            _pm_read(0x00000003, 24)            \
            _pm_read(0x00000004, 32)            \
            _pm_read(0x00000005, 40)            \
            _pm_read(0x00000006, 48)            \
            :                                   \
            : "r"((arr))                        \
            : "memory", "%rax", "%rcx", "%rdx"  \
        );                                       

#define _ph_read_pm_8(arr)                      \
        asm volatile(                           \
            _pm_read(0x00000000, 0)             \
            _pm_read(0x00000001, 8)             \
            _pm_read(0x00000002, 16)            \
            _pm_read(0x00000003, 24)            \
            _pm_read(0x00000004, 32)            \
            _pm_read(0x00000005, 40)            \
            _pm_read(0x00000006, 48)            \
            _pm_read(0x00000007, 56)            \
            :                                   \
            : "r"((arr))                        \
            : "memory", "%rax", "%rcx", "%rdx"  \
        );                                       

#define _ph_read_pm_9(arr)
#define _ph_read_pm_10(arr)
#define _ph_read_pm_11(arr)
#define _ph_read_pm_12(arr)

#define _ph_read_pm_ev(_val_arr)   _ph_read_pm_4(_val_arr);
#define _ph_read_pm_evx(_val_arr)  _ph_read_pm_8(_val_arr);