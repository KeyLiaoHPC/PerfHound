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
 * vt_etag.h
 * Description: Predefined event tag.
 * Author: Key Liao
 * Modified: Jul. 6th, 2020
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

/* Number of etags */
#define _N_ETAG_AARCH64     2
#define _N_ETAG_X64         2

/* Etag ID */

/* Etag name string. */
// Aarch64 etag
char vt_etags_arm[2][32] = {
    "0",
    "VT_NANOSEC"
};

// x86_64 etag
char vt_etags_x64[2][32] = {
    "0",
    "VT_NANOSEC"
};
