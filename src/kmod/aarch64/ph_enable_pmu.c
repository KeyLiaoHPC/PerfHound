/*
 * PerfHound Armv8 PMU enable module (ph_enable_pmu)
 *
 * Opens EL0 access to PMU system registers on each CPU so PHASM probe
 * (ph_pm_aarch64.h) can MRS/MSR counters from user space.
 */
#include <linux/kernel.h>
#include <linux/smp.h>
#include <linux/module.h>
#include <linux/init.h>

#define PMUSER_ON   0x00000001
#define PMCNTEN_ALL 0x8000ffff

static void
enable_counters(void *p)
{
    u32 reg_in = 0, reg_out = 0;

    (void)p;
    reg_in = reg_in | PMUSER_ON;
    asm volatile(
        "msr pmuserenr_el0, %1"     "\n\t"
        "mrs %0, pmuserenr_el0"     "\n\t"
        : "=r" (reg_out)
        : "r" (reg_in)
    );

    reg_in = 0x47;
    asm volatile(
        "msr pmcr_el0, %1"      "\n\t"
        "mrs %0, pmcr_el0"      "\n\t"
        : "=r" (reg_out)
        : "r" (reg_in)
    );

    reg_in = PMCNTEN_ALL;
    asm volatile(
        "msr pmcntenset_el0, %1"    "\n\t"
        "mrs %0, pmcntenset_el0"    "\n\t"
        : "=r" (reg_out)
        : "r" (reg_in)
    );
}

static void
disable_counters(void *p)
{
    u32 reg_in;

    (void)p;
    reg_in = PMCNTEN_ALL;
    asm volatile(
        "msr pmcntenclr_el0, %0"    "\n\t"
        "isb"
        :
        : "r" (reg_in)
    );

    reg_in = 0;
    asm volatile(
        "msr pmcr_el0, %0"          "\n\t"
        "msr pmuserenr_el0, %0"     "\n\t"
        "isb"
        :
        : "r" (reg_in)
    );
}

static int __init
ph_enable_pmu_init(void)
{
    on_each_cpu(enable_counters, NULL, 1);
    printk(KERN_INFO "ph_enable_pmu: Arm user-space PMU access enabled.\n");
    return 0;
}

static void __exit
ph_enable_pmu_exit(void)
{
    on_each_cpu(disable_counters, NULL, 1);
    printk(KERN_INFO "ph_enable_pmu: Arm PMU disabled.\n");
}

MODULE_AUTHOR("Key Liao");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("PerfHound: enable user-mode access to Armv8 PMU counters");
MODULE_VERSION("1.0");
module_init(ph_enable_pmu_init);
module_exit(ph_enable_pmu_exit);
