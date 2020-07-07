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
int vt_init(char *u_data_root, char *u_proj_name);

/**
 * @brief Get and record an pre-defined event reading.
 * @param ctag Code tag, a descriptive word tag for the counting point.
 * @param etag Event tag, an exsited event name in varapi.h.
 * @return int Return error code.
 */
void vt_read(char *ctag, int clen, uint32_t etag1, uint32_t etag2, uint32_t etag3);

/**
 * @brief Record a self-defined event value.
 * 
 * @param ctag 
 * @param etag 
 * @param vt_type 
 * @param val 
 */
void vt_record(char *ctag, int clen, char *etag, int elen, int vt_type, void *val);

void vt_write();

/**
 * @brief Exit vartect api.
 * @return int 
 */
void vt_finalize();

#endif