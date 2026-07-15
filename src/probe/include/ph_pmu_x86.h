/*
 * Internal x86 PMU user-space API (not public).
 *
 * Configures counters via ph_enable_pmu.ko sysfs; rdpmc hot path is in ph_pm_x86_64.h.
 * Prerequisite: sudo insmod .../ph_enable_pmu.ko
 */
#ifndef PH_PMU_X86_H
#define PH_PMU_X86_H

#include <stdint.h>

typedef uint64_t PH_PMU_CFG;
typedef int64_t  PH_PMU_CNT;

#define PH_PMU_ERR_OK              0
#define PH_PMU_ERR_OPEN_SYSFILE   -1
#define PH_PMU_ERR_PWRITE_FAILED  -2
#define PH_PMU_ERR_PWRITE_TOO_FEW -3
#define PH_PMU_ERR_CR4_PCE_NOT_SET -5
#define PH_PMU_ERR_READING_MASKS  -7

int        ph_pmu_init(void);
void       ph_pmu_fini(void);
int        ph_pmu_wr_cfgs(int k, int n, const PH_PMU_CFG *cfg);
int        ph_pmu_wr_cnts(int k, int n, const PH_PMU_CNT *cnt);
PH_PMU_CFG ph_pmu_parse_cfg(const char *s);

#endif /* PH_PMU_X86_H */
