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
 * varapi.h
 * Description: API for PerfHound
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
 * @brief Initialise PerfHound api. This function must be called exactly once before the 
 * variation detection.
 * @param data_dir      Root dir for output data.
 * @param proj_name     Project name, for intra-project data comparison.
 * @return int 
 */
int vt_init(char *u_data_root, char *u_proj_name);

/**
 * @brief Register system events which will be recorded. 
 * @param etags         Array of event names.
 */
int vt_set_evt(const char *etags);

void vt_commit();

/**
 * @brief Get and record an pre-defined event reading.
 * @param grp_id        Group index.
 * @param p_id          Collecting point index.
 * @param uval          Any user-defined value.
 * @param auto_write    Set to 1 if Pfh-Probe automatically detect the buffer and 
 *                      dump to files when the buffer is full.
 * @param read_ev       Set to 1 if collect performance events at this point.  
 * @return              Int. Return error code.
 */
void vt_read(uint32_t grp_id, uint32_t p_id, double uval, int auto_write, int read_ev);

/**
 * @brief Creating a collecting group
 */
int vt_set_grp(uint32_t grp_id, const char *grp_name);

/**
 * @brief Creating tag for a collecting point.
 * 
 */
int vt_set_tag(uint32_t grp_id, uint32_t p_id, const char *name);

/**
 * @brief Write collected data to csv files.
 */
void vt_write();

void vt_strict_sync();

/**
 * @brief Exit Pfh-Probe and release resources.
 * @return  
 */
void vt_clean();

#endif
