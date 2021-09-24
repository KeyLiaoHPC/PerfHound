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
#ifndef PFH_RECBUF_KIB 
#define PFH_RECBUF_KIB  2048      // 4 KiB
#endif
#define PFH_BUF_NMARGIN 4

#define ALIGN           64

/* ======== The END of user config area ======== */


/**
 * Define collecting mode for PFH-Probe
 * PFH_MODE_TS: Timestamp mode. (Default)
 * PFH_MODE_EV: Up to 4 Events.
 * PFH_MODE_EVX: Up to 12 Events.
 */

/* Import performance monitor header after setting mode. */



#ifdef PFH_MODE_EVX

#ifdef __x86_64__
static int pfh_nev_max = 8;
#else
static int pfh_nev_max = 12;
#endif

#else
static int pfh_nev_max = 4;
#endif

#ifdef PFH_MODE_EVX
#define _N_EVARR 12
#else
#define _N_EVARR 4
#endif

/* Pfh-Probe record data type */
typedef struct __attribute__((packed)){
    uint32_t ctag[2];
    uint64_t cy, ns;    
    double uval;        // 32 Bytes
#ifdef USE_PAPI
    long long int ev[_N_EVARR];
#else
    uint64_t ev[_N_EVARR];
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

#ifdef USE_PAPI
#include "pfh_pm_papi.h"

static int pfh_evcodes[12];

#else
#ifdef __x86_64__
#include "pfh_pm_x86_64.h"
#include "pfh_evt_clx.h"

#elif  __aarch64__
#include "pfh_pm_aarch64.h"
#include "pfh_evt_aarch64.h"

#endif // END: #ifdef __x86_64__


static uint64_t pfh_evcodes[12];
#endif // END: #ifdef USE_PAPI


#endif // END: #ifndef __VARAPI_CORE_H__
