/*
 * Shared PerfHound event registration for examples.
 * Compile with -DPH_API_PAPI when linking against a PAPI-built probe.
 * Define PHMPI before including to use phmpi_set_evt.
 */
#ifndef PH_EVENTS_H
#define PH_EVENTS_H

#include <string.h>

#ifdef PHMPI
#define PH_EVT_SET phmpi_set_evt
#else
#define PH_EVT_SET ph_set_evt
#endif

static inline void ph_example_set_events(const char *mode)
{
    if (strcmp(mode, "EV") != 0 && strcmp(mode, "EVX") != 0) {
        return;
    }

#if defined(__aarch64__)
    if (strcmp(mode, "EV") == 0) {
        PH_EVT_SET("CPU_CYCLES");
        PH_EVT_SET("INST_RETIRED");
        PH_EVT_SET("BR_RETIRED");
        PH_EVT_SET("L1D_CACHE");
    } else {
        PH_EVT_SET("CPU_CYCLES");
        PH_EVT_SET("INST_RETIRED");
        PH_EVT_SET("BR_RETIRED");
        PH_EVT_SET("L1D_CACHE");
        PH_EVT_SET("L1I_CACHE");
        PH_EVT_SET("L2D_CACHE");
        PH_EVT_SET("L1D_CACHE_REFILL");
        PH_EVT_SET("L1D_TLB");
        PH_EVT_SET("L1D_TLB_REFILL");
        PH_EVT_SET("MEM_ACCESS");
        PH_EVT_SET("STALL_FRONTEND");
        PH_EVT_SET("STALL_BACKEND");
    }
#elif defined(PH_API_PAPI)
    if (strcmp(mode, "EV") == 0) {
        PH_EVT_SET("CPU_CLK_THREAD_UNHALTED:THREAD_P");
        PH_EVT_SET("INST_RETIRED:ANY_P");
        PH_EVT_SET("UOPS_ISSUED:ANY");
        PH_EVT_SET("UOPS_RETIRED:ALL");
    } else {
        PH_EVT_SET("CPU_CLK_THREAD_UNHALTED:THREAD_P");
        PH_EVT_SET("INST_RETIRED:ANY_P");
        PH_EVT_SET("UOPS_ISSUED:ANY");
        PH_EVT_SET("UOPS_RETIRED:ALL");
        PH_EVT_SET("UOPS_DISPATCHED_PORT:PORT_0");
        PH_EVT_SET("UOPS_DISPATCHED_PORT:PORT_1");
        PH_EVT_SET("UOPS_DISPATCHED_PORT:PORT_2");
        PH_EVT_SET("UOPS_DISPATCHED_PORT:PORT_3");
    }
#else
    if (strcmp(mode, "EV") == 0) {
        PH_EVT_SET("cpu_clk_unhalted.core_clk");
        PH_EVT_SET("inst_retired.any_p");
        PH_EVT_SET("uops_issued.any");
        PH_EVT_SET("uops_retired.all");
    } else {
        PH_EVT_SET("cpu_clk_unhalted.core_clk");
        PH_EVT_SET("inst_retired.any_p");
        PH_EVT_SET("uops_issued.any");
        PH_EVT_SET("uops_retired.all");
        PH_EVT_SET("uops_executed_port.port_0");
        PH_EVT_SET("uops_executed_port.port_1");
        PH_EVT_SET("uops_executed_port.port_2");
        PH_EVT_SET("uops_executed_port.port_3");
    }
#endif
}

#endif /* PH_EVENTS_H */
