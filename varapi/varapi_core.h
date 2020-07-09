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

#ifdef __x86_64__
#include "rec_ts_x86_64.h"
#include "rec_event_x86_64.h"

#elif  defined(__aarch64__)
#include "rec_ts_aarch64.h"
#include "rec_event_aarch64.h"

#endif

/* Limits for Varapi tracing. */
#define _PATH_MAX    4096    // Path max length.
#define _HOST_MAX    253     // Hostname max length.
#define _PROJ_MAX    64      // Project name max length.
#define _TAG_MAX     16      // Tag max length.
#define _NCTAG_MAX   32      // Max number of ctag.
#define _NETAG_MAX   8       // Max number of self-defined etag.

/* MPI and IO settings. */
#define _RANK_PER_IO    64

/* 256 pieces/ 4 Kib for buffering counting messages */
//#define _MSG_BUF_BYTE   4096    // 4 KiB
#define _MSG_BUF_N      512      // 256 records
#define _MSG_LEN        256
#define _TAG_LEN        20

/* CTAG: Counter tag*/
#ifdef  __x86_64__

#define VT_CYCLE        0
#define VT_NANOSEC      1

#elif   __aarch64__

#define VT_CYCLE        0
#define VT_NANOSEC      1

#endif

/* VT_TYPE*/
#ifndef __VT_TYPE__
#define __VT_TYPE__

#define VT_INT      1
#define VT_INT8     2
#define VT_INT16    3
#define VT_INT32    4
#define VT_INT64    5
#define VT_UINT     6
#define VT_UINT8    7
#define VT_UINT16   8
#define VT_UINT32   9
#define VT_UINT64   10
#define VT_FLOAT    11
#define VT_DOUBLE   12

#endif

typedef struct {
    char ctag[32], etag[32]; // ctag is the remark of counting position, etag is event tag.
    u_int64_t val;
} data_t;

#endif