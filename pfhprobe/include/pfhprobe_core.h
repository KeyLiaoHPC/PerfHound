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
#include <limits.h>

/* ======== User config area ======== */
/* Limits for Varapi tracing. */
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
#ifndef PFH_RECBUF_KIB 
#define PFH_RECBUF_KIB  2048      // 4 KiB
#define PFH_BUF_NMARGIN 4
#endif

#define ALIGN           64

#define _MSG_BUF_N      512      // 256 records
#define _MSG_LEN        256
#define _CTAG_LEN       16
/* ======== The END of user config area ======== */


/**
 * Define collecting mode for PFH-Probe
 * PFH_MODE_TS: Timestamp mode. (Default)
 * PFH_MODE_EV: Up to 4 Events.
 * PFH_MODE_EVX: Up to 12 Events.
 */

#ifdef PFH_MODE_TS
#undef PFH_MODE_EV
#undef PFH_MODE_EVX
#endif

#ifdef PFH_MODE_EV
#undef PFH_MODE_TS
#undef PFH_MODE_EVX
#define _N_EV 4
#endif

#ifdef PFH_MODE_EVX
#undef PFH_MODE_TS
#undef PFH_MODE_EV

#ifdef __x86_64__
#define _N_EV 8
#elif __aarch64__
#define _N_EV 12
#endif // END: #ifdef VT_NODE_TS
#endif

/* Import performance monitor header after setting mode. */
#ifdef __x86_64__
#include "pfh_pm_x86_64.h"
#include "pfh_evt_clx.h"

#elif  __aarch64__
#include "pfh_pm_aarch64.h"
#include "pfh_event_aarch64.h"

#endif // END: #ifdef __x86_64__


/* Pfh-Probe record data type */
typedef struct {
    uint32_t ctag[2];
    uint64_t cy, ns;    
    double uval;        // 32 Bytes
#ifdef PFH_MODE_EV
    uint64_t ev[4];
#elif PFH_MODE_EVX
    uint64_t ev[12];
#endif
} rec_t;


/**
 * Process info. 
 */
typedef struct {
    int rank, nrank, cpu, head, iorank;
    char host[_HOST_MAX];
} proc_t;

#ifdef _N_EV 
uint64_t pfh_evcodes[12];
#endif
extern rec_t *pfh_precs; // raw data.
extern proc_t pfh_pinfo;
extern int pfh_nevt;


/* MPI Wrapper in vt_mpi.c */
#ifdef USE_MPI
void pfh_recover_bias();
void pfh_bcast_tstamp(char *timestr);
void pfh_get_cur_bias();
void pfh_get_bias(int r0, int r1);
int  pfh_get_data(uint32_t rank, uint32_t *count, rec_t *data);
void pfh_get_rank(uint32_t *nrank, uint32_t *myrank);
void pfh_io_barrier();
void pfh_mpi_clean();
void pfh_newtype();
void pfh_send_data(uint32_t count, rec_t *data);
int  pfh_sync_mpi_info(char *projpath, int *run_id, uint32_t *head, int *iorank, 
                      uint32_t *vt_iogrp_size, uint32_t *vt_iogrp_grank, uint32_t *vt_iogrp_gcpu);
void pfh_tsync();
void pfh_world_barrier();
#endif

#endif // END: #ifndef __VARAPI_CORE_H__