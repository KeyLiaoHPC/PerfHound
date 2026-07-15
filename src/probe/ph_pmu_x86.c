/*
 * x86 PMU sysfs client for ph_enable_pmu.ko
 *
 * Opens /sys/module/ph_enable_pmu/{config,counts,masks} for counter programming
 * during ph_pm_x86_64.h init/commit; hot-path reads use rdpmc directly.
 */
#define _GNU_SOURCE

#include "ph_pmu_x86.h"
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

static int cfg_fd = -1;
static int msk_fd = -1;
static int cnt_fd = -1;
static int cr4_fd = -1;

int
ph_pmu_init(void)
{
    cfg_fd = open("/sys/module/ph_enable_pmu/config", O_RDWR | O_CLOEXEC);
    msk_fd = open("/sys/module/ph_enable_pmu/masks", O_RDONLY | O_CLOEXEC);
    cnt_fd = open("/sys/module/ph_enable_pmu/counts", O_RDWR | O_CLOEXEC);
    cr4_fd = open("/sys/module/ph_enable_pmu/cr4.pce", O_RDONLY | O_CLOEXEC);

    if (cfg_fd < 0 || msk_fd < 0 || cnt_fd < 0 || cr4_fd < 0) {
        ph_pmu_fini();
        return PH_PMU_ERR_OPEN_SYSFILE;
    }

    /* User-space rdpmc must be enabled or ph_read rdpmc will fail */
    {
        unsigned char buf[1];
        int n = read(cr4_fd, buf, sizeof(buf));
        close(cr4_fd);
        cr4_fd = -1;
        if (n != 1 || buf[0] != '1')
            return PH_PMU_ERR_CR4_PCE_NOT_SET;
    }

    {
        uint64_t masks[7];
        if (pread(msk_fd, masks, sizeof(masks), 0) != (ssize_t)sizeof(masks))
            return PH_PMU_ERR_READING_MASKS;
    }

    return PH_PMU_ERR_OK;
}

void
ph_pmu_fini(void)
{
    if (cfg_fd >= 0) {
        close(cfg_fd);
        cfg_fd = -1;
    }
    if (msk_fd >= 0) {
        close(msk_fd);
        msk_fd = -1;
    }
    if (cnt_fd >= 0) {
        close(cnt_fd);
        cnt_fd = -1;
    }
    if (cr4_fd >= 0) {
        close(cr4_fd);
        cr4_fd = -1;
    }
}

int
ph_pmu_wr_cfgs(int k, int n, const PH_PMU_CFG *cfg)
{
    ssize_t wr_size = sizeof(*cfg) * (size_t)n;
    ssize_t actual;

    actual = pwrite(cfg_fd, cfg, wr_size, (off_t)k * (off_t)sizeof(*cfg));
    if (actual == -1)
        return PH_PMU_ERR_PWRITE_FAILED;
    if (actual < wr_size)
        return PH_PMU_ERR_PWRITE_TOO_FEW;
    return PH_PMU_ERR_OK;
}

int
ph_pmu_wr_cnts(int k, int n, const PH_PMU_CNT *cnt)
{
    ssize_t wr_size = sizeof(*cnt) * (size_t)n;
    ssize_t actual;

    actual = pwrite(cnt_fd, cnt, wr_size, (off_t)k * (off_t)sizeof(*cnt));
    if (actual == -1)
        return PH_PMU_ERR_PWRITE_FAILED;
    if (actual < wr_size)
        return PH_PMU_ERR_PWRITE_TOO_FEW;
    return PH_PMU_ERR_OK;
}
