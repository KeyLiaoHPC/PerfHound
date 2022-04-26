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
 * varapi_core.h
 * Description: Headers of internal type and macros for varapi.
 * Author: Key Liao
 * Modified: May. 28th, 2020
 * Email: keyliaohpc@gmail.com
 * =================================================================================
 */


#ifndef __VARAPI_CORE_H__
#define __VARAPI_CORE_H__

#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>

/* ======== User config area ======== */
/* Limits for Varapi tracing. */
#define _HOST_MAX    253     // Hostname max length.
#define _PROJ_MAX    64      // Project name max length.
#define _TAG_MAX     16      // Tag max length.
#define _NCTAG_MAX   32      // Max number of ctag.
#define _NETAG_MAX   8       // Max number of self-defined etag.

/* MPI and IO settings. */
#define _RANK_PER_IO    20
#define _SYNC_NS_OFFSET 1e4     // Offset of sync, 1 sec for default.

/* 256 pieces/ 4 Kib for buffering counting messages */
#ifndef PFH_OPT_BUFSIZE 
#define PFH_OPT_BUFSIZE  2048      // 4 KiB
#endif

#define ALIGN           64

/* ======== The END of user config area ======== */


/**
 * Define record size for PerfHound.
 * PFH_OPT_TS: 32Bytes. Timestamp mode. (Default)
 * PFH_OPT_EV: 64Bytes. Up to 4 Events.
 * PFH_OPT_EVX: 128Bytes. Up to 12 Events.
 */

#ifdef PFH_OPT_EVX

#ifdef __x86_64__
static int pfh_nev_max = 8;
#else
static int pfh_nev_max = 12;
#endif

#else

static int pfh_nev_max = 4;

#endif

#ifdef PFH_OPT_EVX
#define PFH_NEV 12
#else
#define PFH_NEV 4
#endif

/* Pfh-Probe record data type */
typedef struct __attribute__((packed)){
    uint32_t ctag[2];
    int64_t ns, cy;
    double uval;        // 32 Bytes
#ifdef PFH_OPT_PAPI
    int64_t ev[PFH_NEV];
#else
    uint64_t ev[PFH_NEV];
#endif
} rec_t;


/**
 * Process info. 
 */
typedef struct {
    int rank, nrank, cpu, head, iorank;
    char host[_HOST_MAX];
} proc_t;

/**
 * Host list and nranks on a host
 */
typedef struct {
    int nrank;
    char host[_HOST_MAX];
    int *prank;
} hinfo_t;

extern rec_t *pfh_precs; // raw data.
extern proc_t pfh_pinfo;
extern int pfh_nev;

#ifdef PFH_OPT_PAPI

#include "pfh_pm_papi.h"
static int pfh_evcodes[PFH_NEV];

#else

#ifdef __x86_64__
#include "pfh_pm_x86_64.h"
#include "pfh_evt_clx.h"
#elif  __aarch64__
#include "pfh_pm_aarch64.h"
#include "pfh_evt_aarch64.h"
#endif // END: #ifdef __x86_64__

static uint64_t pfh_evcodes[PFH_NEV];

#endif // END: #ifdef PFH_OPT_PAPI

#endif // END: #ifndef __VARAPI_CORE_H__
