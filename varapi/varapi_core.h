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

#ifndef __VARAPI_CORE_H__
#define __VARAPI_CORE_H__

#include <stdint.h>
#include <stdio.h>
#include <time.h>


/* ======== User config area ======== */
/* Limits for Varapi tracing. */
#define _PATH_MAX    4096    // Path max length.
#define _HOST_MAX    253     // Hostname max length.
#define _PROJ_MAX    64      // Project name max length.
#define _TAG_MAX     16      // Tag max length.
#define _NCTAG_MAX   32      // Max number of ctag.
#define _NETAG_MAX   8       // Max number of self-defined etag.

/* MPI and IO settings. */
#define _AUTO_WRITE     1
#define _RANK_PER_IO    20
#define _SYNC_NS_OFFSET 1e4     // Offset of sync, 1 sec for default.

/* 256 pieces/ 4 Kib for buffering counting messages */
#ifndef _MSG_BUF_KIB
#define _MSG_BUF_KIB    16384    // 4 KiB
#endif

#define _MSG_BUF_N      512      // 256 records
#define _MSG_LEN        256
#define _CTAG_LEN       16
/* ======== The END of user config area ======== */

/* CTAG: Counter tag*/
#ifdef  __x86_64__

#define VT_CYCLE        0
#define VT_NANOSEC      1

#elif   __aarch64__

#define VT_CYCLE        0
#define VT_NANOSEC      1

#endif  // END: #ifdef __x86_64__

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

#endif // END: #ifndef __VT_TYPE__

/* VarTect Mode */
// VT_MODE_TS:          Timestamp mode.     [Default]
// VT_MODE_SHORT_EV:    Up to 4 Events.
// VT_MODE_LONG_EV:     Up to 12 Events.
#ifndef _MODE_SET
#define _MODE_SET

#ifndef VT_MODE_TS
#define VT_MODE_TS
#endif

// Collecting performance events. 
#ifdef VT_MODE_EV
#undef VT_MODE_TS
#define _N_EV 4

#endif // END: #ifdef VT_MODE_EV
#endif // #ifndef _MODE_SET

/* Import performance monitor header after setting mode. */
#ifndef __PM_HEADER__
#define __PM_HEADER__
#ifdef __x86_64__
#include "vt_pm_x86_64.h"
#include "vt_event_x86_64.h"

#elif  __aarch64__
#include "vt_pm_aarch64.h"
#include "vt_event_aarch64.h"

#endif // END: #ifdef __x86_64__
#endif // END: #ifndef __PM_HEADER__


/* VarAPI record data type */
typedef struct {
    uint32_t ctag[2];
    uint64_t cy, ns;    
    double uval;        // 32 Bytes
#ifdef _N_EV
    uint64_t    ev[_N_EV];
#endif
} data_t;

/* Extern file-operating functions in file_op.c*/
int  vt_getstamp(char *hostpath, char *timestr, int *run_id);
void vt_log(FILE *fp, char *fmt, ...);
int  vt_mkdir(char *path);
void vt_putstamp(char *hostpath, char *timestr);
int  vt_touch(char *path, char *mode);

/* MPI Wrapper in vt_mpi.c */
#ifdef USE_MPI
void vt_recover_bias();
void vt_bcast_tstamp(char *timestr);
void vt_get_cur_bias();
void vt_get_bias(int r0, int r1);
int  vt_get_data(uint32_t rank, uint32_t *count, data_t *data);
void vt_get_rank(uint32_t *nrank, uint32_t *myrank);
void vt_io_barrier();
void vt_mpi_clean();
void vt_newtype();
void vt_send_data(uint32_t count, data_t *data);
int  vt_sync_mpi_info(char *projpath, int *run_id, uint32_t *head, int *iorank, 
                      uint32_t *vt_iogrp_size, uint32_t *vt_iogrp_grank, uint32_t *vt_iogrp_gcpu);
void vt_tsync();
void vt_world_barrier();
#endif

#endif // END: #ifndef __VARAPI_CORE_H__
