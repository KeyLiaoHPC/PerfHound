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
 * rec_ts_x86_64.h
 * Description: Macros for timer on x86_64.
 * Author: Key Liao
 * Modified: Jul. 2nd, 2020
 * Email: keyliaohpc@gmail.com
 * =================================================================================
 */
//==================================================================================

#include "papi.h"

static int evset = PAPI_NULL;

#define _pfh_init_ts    \
    PAPI_library_init(PAPI_VER_CURRENT);    \
    PAPI_create_eventset(&evset);       

#define _pfh_fini_ts    \
    PAPI_shutdown();

#ifdef _PFH_MPI

#ifdef __x86_64__
#define _pfh_init_cy
    // pfhmpi_set_evt("CPU_CLK_UNHALTED");
#elif  __aarch64__
#define _pfh_init_cy
    // pfhmpi_set_evt("CPU_CYCLES");
#else
#define _pfh_init_cy
#endif

#else

#ifdef __x86_64__
#define _pfh_init_cy
    // pfh_set_evt("CPU_CLK_UNHALTED");
#elif  __aarch64__
#define _pfh_init_cy
    // pfh_set_evt("CPU_CYCLES");
#else
#define _pfh_init_cy
#endif

#endif


#define _pfh_reg_save                                  


#define _pfh_reg_restore                                


/* Read actual clock cycle from CPU_CLK_UNHALTED.THREAD */
#define _pfh_read_cy(_cy)   \
    (_cy) = PAPI_get_real_cyc();


/* Read virtual timer */
#ifdef USE_SYSCALL_TS
#include <sys/syscall.h>
#define _pfh_read_ns(_ns)   \
    struct timespec ts;                                 \
    syscall(__NR_clock_gettime, CLOCK_REALTIME, &ts);   \
    (_ns) = ts.tv_sec * 1e9 + ts.tv_nsec;
                            
#else
#define _pfh_read_ns(_ns)   \
    (_ns) = PAPI_get_real_nsec();
#endif

#define _pfh_parse_event(_code, _evstr)     \
    if (PAPI_query_named_event(_evstr)) {                   \
        (_code) = 0xFFFFFFFF;                               \
    } else {                                                \
        PAPI_event_name_to_code((_evstr), &(_code));        \
    }

#define _pfh_config_event(_code_arr, _nevt)             \
    if ((_nevt)) {                                      \
        PAPI_add_events(evset, (_code_arr), (_nevt));   \
    }                                                   \
    PAPI_start(evset);                                           


/* Read PMC */

#define _pfh_read_pm_1(_arr)    \
        PAPI_read(evset, _arr);

#define _pfh_read_pm_2(_arr)    \
        PAPI_read(evset, _arr);
        
#define _pfh_read_pm_3(_arr)    \
        PAPI_read(evset, _arr);

#define _pfh_read_pm_4(_arr)    \
        PAPI_read(evset, _arr);

#define _pfh_read_pm_5(_arr)    \
        PAPI_read(evset, _arr);

#define _pfh_read_pm_6(_arr)    \
        PAPI_read(evset, _arr);

#define _pfh_read_pm_7(_arr)    \
        PAPI_read(evset, _arr);

#define _pfh_read_pm_8(_arr)    \
        PAPI_read(evset, _arr);

#define _pfh_read_pm_9(_arr)    \
        PAPI_read(evset, _arr);

#define _pfh_read_pm_10(_arr)   \
        PAPI_read(evset, _arr);

#define _pfh_read_pm_11(_arr)   \
        PAPI_read(evset, _arr);

#define _pfh_read_pm_12(_arr)   \
        PAPI_read(evset, _arr);

#define _pfh_read_pm_ev(_val_arr)   _pfh_read_pm_4(_val_arr);
#define _pfh_read_pm_evx(_val_arr)  _pfh_read_pm_8(_val_arr);