/*
 * =================================================================================
 * VarTect - Detecting and analyzing performance variation in parallel program
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

#include "libpfc.h"

#define _X86_TSC_HZ 2.49414

#define _pfc_read(rcx)                \
    "\n\tmov      $"#rcx", %%rcx                 "  \
    "\n\trdpmc                                   "  \
    "\n\tshl      $32,     %%rdx                 "  \
    "\n\tor       %%rax,   %%rdx                 "  \
    "\n\tmov      %%rdx,   %0                    "  \
    "\n\t"

/* Init fixed-function registers. */
// This call is based on https://github.com/obilaniu/libpfc
#define _vt_init_ts(_nspt)                                  \
    PFC_CNT pfc_cnts[7] = {0, 0, 0, 0, 0, 0, 0};            \
    PFC_CFG pfc_cfgs[7] = {2, 2, 2, 0, 0, 0, 0};            \
    const PFC_CNT pfc_zero_cnts[7] = {0, 0, 0, 0, 0, 0, 0}; \
    do {                                                    \
        int cpu = sched_getcpu();                           \
        if (pfcPinThread(cpu)) {                            \
            exit(1);                                        \
        }                                                   \
        if (pfcInit() != 0) {                               \
            printf("Failed to load pfc file.\n");           \
            fflush(stdout);                                 \
            exit(1);                                        \
        }                                                   \
        pfcWrCfgs(0, 7, pfc_cfgs);                          \
        pfcWrCnts(0, 7, pfc_zero_cnts);                     \
    } while(0);                                             \
    _nspt = 1 / _X86_TSC_HZ;                            

#define _vt_fini_ts     \
    pfcFini()

/* Read actual clock cycle from CPU_CLK_UNHALTED.THREAD */
#define _vt_read_cy(_cy)                                \
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
#define _vt_read_ns(_ns)    syscall(__NR_clock_gettime, CLOCK_REALTIME, &ts);   \
                            (_ns) = ts.tv_sec * 1e9 + ts.tv_nsec;
                            
#else
#define _vt_read_ns(_ns)                                \
    do {                                                \
        register uint64_t cy;                           \
        asm volatile(                                   \
            "\n\tRDTSCP"                                \
            "\n\tshl $32, %%rdx"                        \
            "\n\tor  %%rax, %%rdx"                      \
            "\n\tmov %%rdx, %0"                         \
            "\n\t"                                      \
            :"=r" (cy)                                  \
            :                                           \
            : "memory", "%rax", "%rdx");                \
        _ns = cy;                                       \
    } while(0)

//    do {                                                \
//        register uint64_t hi, lo, cy;                   \
//        asm volatile(                                   \
//            "CPUID"         "\n\t"                      \
//            "RDTSCP"        "\n\t"                      \
//            "mov %%rdx, %0" "\n\t"                      \
//            "mov %%rax, %1" "\n\t"                      \
//            :"=r" (hi), "=r" (lo)                       \
//            :                                           \
//            :"%rax", "%rbx", "%rcx", "%rdx");           \
//        cy = hi << 32 | lo;                             \
//        _ns = (double)cy / 2.494140000;                 \
//    } while(0)

#endif

/* User-defined event reading. */
#define _vt_read_uev(_tp, _void_ptr, _val)  _val = (double)(*((_tp *)_void_ptr));