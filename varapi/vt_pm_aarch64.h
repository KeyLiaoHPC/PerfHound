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
 * rec_ts_aarch64.h
 * Description: Macros for timer on aarch64.
 * Author: Key Liao
 * Modified: Jul. 2nd, 2020
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

/* Init timer */
#ifdef USE_CNTVCT
#define _vt_init_ns     uint32_t freq;  \
                        asm volatile("mrs %0, cntfreq_el0"  "\n\t": "=r" (freq)::); \
                        nspt = 1 / (freq * 1e-9);

#else
#define _vt_init_ns     asm volatile("NOP"  "\n\t":::);

#endif // END: #ifdef USE_CNTVCT

/* Init Armv8 PMU settings */
#define _vt_cy_init    uint64_t _pmu_val0 = 0;             \
                        _pmu_val0 = _pmu_val0 | (1 << 31);  \
                        asm volatile(                       \
                            "mrs x22, pmcr_el0" "\n\t"      \
                            "orr x22, x22, #0x7"    "\n\t"  \
                            "msr pmcr_el0, x22"     "\n\t"  \
                            "msr pmcntenset_el0, %0" "\n\t" \
                            :                               \
                            : "r" (_pmu_val0)               \
                            : "x22"                         \
                        );
/* Read cycle */
#define _vt_read_cy(_cy)  asm volatile("mrs %0, pmccntr_el0"     "\n\t": "=r" (_cy)::);

/* Read virtual timer */
#ifdef USE_CNTVCT
#define _vt_read_ns(_ns)    asm volatile("mrs %0, cntvct_el0"      "\n\t": "=r" (_ns)::); \
                            _ns *= nspt;
#else
#define _vt_read_ns(_ns)    clock_gettime(CLOCK_MONOTONIC, &ts);    \
                            (_ns) = ts.tv_sec * 1e9 + ts.tv_nsec;
#endif

/* PMU reading */
#define _PMSET_REG(_id) "pmevtyper"#_id"_el0"
#define _PMVAL_REG(_id) "pmevcntr"#_id"_el0"
#define _vt_set_pmreg(_id, _val)    asm volatile("msr " _PSETREG(_id) ", %0": : "r" (_val));
#define _vt_read_pm(_id, _val)      asm volatile("mrs %0, " _PMVAL_REG(_id) "\n\t": "=r" (_val));

#define _vt_read_pm_1(_val_arr)     _vt_read_pm(0, (_val_arr)[0]);
#define _vt_read_pm_2(_val_arr)     _vt_read_pm_1(_val_arr);        \
                                    _vt_read_pm(1, (_val_arr)[1]);
#define _vt_read_pm_3(_val_arr)     _vt_read_pm_2(_val_arr);        \
                                    _vt_read_pm(2, (_val_arr)[2]);
#define _vt_read_pm_4(_val_arr)     _vt_read_pm_3(_val_arr);        \
                                    _vt_read_pm(3, (_val_arr)[3]);
#define _vt_read_pm_5(_val_arr)     _vt_read_pm_4(_val_arr);        \
                                    _vt_read_pm(4, (_val_arr)[4]);
#define _vt_read_pm_6(_val_arr)     _vt_read_pm_5(_val_arr);        \
                                    _vt_read_pm(5, (_val_arr)[5]);
#define _vt_read_pm_7(_val_arr)     _vt_read_pm_6(_val_arr);        \
                                    _vt_read_pm(6, (_val_arr)[6]);
#define _vt_read_pm_8(_val_arr)     _vt_read_pm_7(_val_arr);        \
                                    _vt_read_pm(7, (_val_arr)[7]);
#define _vt_read_pm_9(_val_arr)     _vt_read_pm_8(_val_arr);        \
                                    _vt_read_pm(8, (_val_arr)[8]);
#define _vt_read_pm_10(_val_arr)    _vt_read_pm_9(_val_arr);        \
                                    _vt_read_pm(9, (_val_arr)[9]);
#define _vt_read_pm_11(_val_arr)    _vt_read_pm_10(_val_arr);       \
                                    _vt_read_pm(10, (_val_arr)[10]);
#define _vt_read_pm_12(_val_arr)    _vt_read_pm_11(_val_arr);       \
                                    _vt_read_pm(11, (_val_arr)[11]);

/* User-defined event reading. */
#define _vt_read_uev(_tp, _void_ptr, _val)  _val = (double)(*((_tp *)_void_ptr));
