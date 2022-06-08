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
 * rec_ts_aarch64.h
 * Description: Macros for timer on aarch64.
 * Author: Key Liao
 * Modified: Jul. 2nd, 2020
 * Email: keyliaohpc@gmail.com
 * =================================================================================
 */
//==================================================================================

#define _pfh_init_ts

#define _pfh_fini_ts


/* Init Armv8 PMU settings */
#define _pfh_init_cy                        \
    do {                                    \
        uint64_t _pmu_val0 = 0;             \
        _pmu_val0 = _pmu_val0 | (1 << 31);  \
        asm volatile(                       \
            "mrs x22, pmcr_el0" "\n\t"      \
            "orr x22, x22, #0x7"    "\n\t"  \
            "msr pmcr_el0, x22"     "\n\t"  \
            "msr pmcntenset_el0, %0" "\n\t" \
            :                               \
            : "r" (_pmu_val0)               \
            : "x22"                         \
        );                                  \
    } while(0);


/* Read cycle */
#define _pfh_read_cy(_cy)  asm volatile("mrs %0, pmccntr_el0\n\t" : "=r" (_cy)::"memory");

/* Read virtual timer */
#ifdef USE_CLOCK_GETTIME
#define _pfh_read_ns(_ns)    clock_gettime(CLOCK_MONOTONIC, &ts);    \
                            (_ns) = ts.tv_sec * 1e9 + ts.tv_nsec;
#elif USE_SYSCALL_GETTIME
#include <sys/syscall.h>
#define _pfh_read_ns(_ns)    syscall(__NR_clock_gettime, CLOCK_REALTIME, &ts);   \
                            (_ns) = ts.tv_sec * 1e9 + ts.tv_nsec;
#else
#define _pfh_read_ns(_ns)    asm volatile("isb; mrs %0, cntvct_el0\n\t" : "=r" (_ns)::"memory");

#endif


/* PMU reading */
#define _PMSET_REG(_id) "pmevtyper"#_id"_el0"
#define _PMVAL_REG(_id) "pmevcntr"#_id"_el0"
#define _pfh_set_pmreg(_id, _val)    asm volatile("msr " _PMSET_REG(_id) ", %0": : "r" (_val));
#define _pfh_read_pm(_id, _val)      asm volatile("mrs %0, " _PMVAL_REG(_id) "\n\t": "=r" (_val)::"memory");

/* Parsing string events to hex event code. */
#define _pfh_parse_event(_code, _evstr)                                     \
    do {                                                                    \
        int ii = 0, len;                                                    \
        _code = 0xFFFFFFFF;                                                 \
        while (( len = strlen(aarch64_events[ii].name) )) {                 \
            if (len != strlen(_evstr)) {                                    \
                ii ++;                                                      \
                continue;                                                   \
            }                                                               \
            if (strncasecmp(_evstr, aarch64_events[ii].name, len) == 0) {   \
                _code = (uint64_t)aarch64_events[ii].event_code;            \
                break;                                                      \
            }                                                               \
            ii ++;                                                          \
        }                                                                   \
    } while(0);

/* 
 * Set event in PMEVTYPER<n>_EL0, clear overflow bit in PMOVSCLR_EL0, 
 * reset PMEVCNTR<n>_EL0 to zero. 
 * 
 */
#ifdef PFH_OPT_EVX

#define _pfh_config_event(_code_arr, _nevt)     \
    if ((_nevt)) {                              \
        do {                                    \
            uint32_t _pmu_val0 = 0xFFFFFFFF;    \
            _pfh_set_pmreg(0, _code_arr[0]);    \
            _pfh_set_pmreg(1, _code_arr[1]);    \
            _pfh_set_pmreg(2, _code_arr[2]);    \
            _pfh_set_pmreg(3, _code_arr[3]);    \
            _pfh_set_pmreg(4, _code_arr[4]);    \
            _pfh_set_pmreg(5, _code_arr[5]);    \
            _pfh_set_pmreg(6, _code_arr[6]);    \
            _pfh_set_pmreg(7, _code_arr[7]);    \
            _pfh_set_pmreg(8, _code_arr[8]);    \
            _pfh_set_pmreg(9, _code_arr[9]);    \
            _pfh_set_pmreg(10, _code_arr[10]);  \
            _pfh_set_pmreg(11, _code_arr[11]);  \
            asm volatile(                       \
                "mrs x22, pmcr_el0"     "\n\t"  \
                "orr x22, x22, #0x2"    "\n\t"  \
                "msr pmcr_el0, x22"     "\n\t"  \
                "msr pmovsclr_el0, %0"  "\n\t"  \
                :                               \
                : "r" (_pmu_val0)               \
                : "memory", "x22"               \
            );                                  \
        } while(0);                             \
    }                                           \

#else

#define _pfh_config_event(_code_arr, _nevt)     \
    if ((_nevt)) {                              \
        do {                                    \
            uint32_t _pmu_val0 = 0xFFFFFFFF;    \
            _pfh_set_pmreg(0, _code_arr[0]);    \
            _pfh_set_pmreg(1, _code_arr[1]);    \
            _pfh_set_pmreg(2, _code_arr[2]);    \
            _pfh_set_pmreg(3, _code_arr[3]);    \
            asm volatile(                       \
                "mrs x22, pmcr_el0"     "\n\t"  \
                "orr x22, x22, #0x2"    "\n\t"  \
                "msr pmcr_el0, x22"     "\n\t"  \
                "msr pmovsclr_el0, %0"  "\n\t"  \
                :                               \
                : "r" (_pmu_val0)               \
                : "memory", "x22"               \
            );                                  \
        } while(0);                             \
    }                                           \

#endif // END: #ifdef PFH_OPT_EVX


#define _pfh_read_pm_1(_val_arr)    _pfh_read_pm(0, (_val_arr)[0]);

#define _pfh_read_pm_2(_val_arr)    _pfh_read_pm_1(_val_arr);        \
                                    _pfh_read_pm(1, (_val_arr)[1]);

#define _pfh_read_pm_3(_val_arr)    _pfh_read_pm_2(_val_arr);        \
                                    _pfh_read_pm(2, (_val_arr)[2]);

#define _pfh_read_pm_4(_val_arr)    _pfh_read_pm_3(_val_arr);        \
                                    _pfh_read_pm(3, (_val_arr)[3]);

#define _pfh_read_pm_5(_val_arr)    _pfh_read_pm_4(_val_arr);        \
                                    _pfh_read_pm(4, (_val_arr)[4]);

#define _pfh_read_pm_6(_val_arr)    _pfh_read_pm_5(_val_arr);        \
                                    _pfh_read_pm(5, (_val_arr)[5]);

#define _pfh_read_pm_7(_val_arr)    _pfh_read_pm_6(_val_arr);        \
                                    _pfh_read_pm(6, (_val_arr)[6]);

#define _pfh_read_pm_8(_val_arr)    _pfh_read_pm_7(_val_arr);        \
                                    _pfh_read_pm(7, (_val_arr)[7]);

#define _pfh_read_pm_9(_val_arr)    _pfh_read_pm_8(_val_arr);        \
                                    _pfh_read_pm(8, (_val_arr)[8]);

#define _pfh_read_pm_10(_val_arr)   _pfh_read_pm_9(_val_arr);        \
                                    _pfh_read_pm(9, (_val_arr)[9]);

#define _pfh_read_pm_11(_val_arr)   _pfh_read_pm_10(_val_arr);        \
                                    _pfh_read_pm(10, (_val_arr)[10]);

#define _pfh_read_pm_12(_val_arr)   _pfh_read_pm_11(_val_arr);        \
                                    _pfh_read_pm(11, (_val_arr)[11]);

#define _pfh_read_pm_ev(_val_arr)   _pfh_read_pm_4(_val_arr);
#define _pfh_read_pm_evx(_val_arr)  _pfh_read_pm_12(_val_arr);