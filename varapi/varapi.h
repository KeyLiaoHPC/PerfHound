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
 * varapi.h
 * Description: API for vartect
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

#ifndef __VARAPI_H__
#define __VARAPI_H__
#include <stdint.h>

/* Performance monitor header. */
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

/* VT_TYPE*/
#ifndef __VT_TYPE__
#define __VT_TYPE__

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

#endif // END #ifndef __VT_TYPE__

/* API: Varapi application interfaces */
/**
 * @brief Initialise vartect api. This function must be called exactly once before the 
 * variation detection.
 * @param data_dir  Root dir for output data.
 * @param proj_name Project name, for intra-project data comparison.
 * @return int 
 */
int vt_init(char *u_data_root, char *u_proj_name, uint32_t *u_etags);

/**
 * @brief Register system events which will be recorded. 
 * @param etag Array of event code.
 * @param nev  Actual number of events, should be less or equal to _N_EV.
 */
void vt_set_ev(uint32_t *etag, int nev);
void vt_set_uev(char *uetag, void *pval, int vt_type);
void vt_commit();

/**
 * @brief Get and record an pre-defined event reading.
 * @param ctag Code tag, a descriptive word tag for the counting point.
 * @param etag Event tag, an exsited event name in varapi.h.
 * @param auto_write Write to file or cover from beginnning when buffer is full.
 * @return int Return error code.
 */
void vt_read(uint32_t group_tag, uint32_t point_tag, double uval, int auto_write, int read_ev, int read_uev);

int vt_newtag(uint32_t group_tag, uint32_t point_tag, const char *name);

void vt_write();

/**
 * @brief Exit VarAPI and release resources.
 * @return  
 */
void vt_clean();

#endif
