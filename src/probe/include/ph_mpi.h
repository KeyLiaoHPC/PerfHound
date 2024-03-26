/**
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
 * ph_mpi.h
 * Description: API for PH-Probe
 * Author: Key Liao
 * Modified: Jul. 28th, 2021
 * Email: keyliao@sjtu.edu.cn
 * =================================================================================
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {

#endif
/** 
 * Initialise PH-Probe. This function must be called exactly once before the 
 * variation detection.
 * @param data_dir      Root dir for output data.
 * @param proj_name     Project name, for intra-project data comparison.
 * @return int 
 */
int phmpi_init(char *path);

/**
 * Register system events which will be recorded. 
 * @param etags         Array of event names.
 */
int phmpi_set_evt(const char *etags);

void phmpi_commit();

/**
 * Get and record an pre-defined event reading.
 * @param grp_id        uint32. Group index.
 * @param p_id          uint32. Collecting point index.
 * @return              int. Return error code.
 */
void phmpi_fastread(uint32_t grp_id, uint32_t p_id, double uval);

void phmpi_read(uint32_t grp_id, uint32_t p_id, double uval);


/**
 * Creating tag for a collecting point.
 * 
 */
int phmpi_set_tag(uint32_t grp_id, uint32_t p_id, char *name);

/**
 * Write collected data to csv files.
 * @param bufsize	uint64. The buffer size in Bytes. Write when the size of data
 * 			fills the buffer. Force write when bufsize=0.
 */
void phmpi_dump();


/**
 * Exit PH-Probe and release resources.
 */
void phmpi_finalize();

#ifdef __cplusplus
}
#endif