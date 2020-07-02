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

/* Init timer */
#define _vt_init_ns

/* Read cycle */
#define _vt_read_cy(_cy)    uint64_t hi lo;                         \
                            asm volatile(                           \
                                "CPUID"         "\n\t"              \
                                "RDTSCP"        "\n\t"              \
                                "mov %%rdx, %0" "\n\t"              \
                                "mov %%rax, %1" "\n\t"              \
                                :"=r" (hi), "=r" (lo)               \
                                :                                   \
                                :"%rax", "%rbx", "%rcx", "%rdx");   \
                            (_cy) = hi << 32 | lo;



/* Read virtual timer */
#define _vt_read_ns(_ns)    clock_gettime(CLOCK_MONOTONIC, &ts);    \
                            (_ns) = ts.tv_sec * 1e9 + ts.tv_nsec;