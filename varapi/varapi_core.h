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
 * varapi_core.h
 * Description: Headers of internal type and macros for varapi.
 * Author: Key Liao
 * Modified: May. 28th, 2020
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

#include <stdint.h>
#include <time.h>

#ifndef __VARAPI_CORE_H__
#define __VARAPI_CORE_H__

#include "vt_counter.h"

#define PATH_MAX    4096
#define HOST_MAX    253
#define PROJ_MAX    64

/* 256 pieces/ 4 Kib for buffering counting messages */
#define _MSG_BUF_BYTE   4096    // 4 KiB
#define _MSG_BUF_N      256     // 256 records

/* CTAG: Counter tag*/
#ifdef  __x86_64__

#define VT_CYCLE        0
#define VT_NANOSEC      1

#elif   __aarch64__

#define VT_CYCLE        0
#define VT_NANOSEC      1

#endif

/* VT_TYPE*/
#define VT_INT      0
#define VT_INT8     1
#define VT_INT16    2
#define VT_INT32    3
#define VT_INT64    4
#define VT_UINT     5
#define VT_UINT8    6
#define VT_UINT16   7
#define VT_UINT32   8
#define VT_UINT64   9
#define VT_FLOAT    10
#define VT_DOUBLE   11

typedef struct {
    uint32_t ctag, etag; // ctag is the remark of counting position, etag is event tag.
    uint64_t val;
} data_t;

typedef struct {
    int rank;
    int cpu;
    int hostid;
} rank_t;

#endif