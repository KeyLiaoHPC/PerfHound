/*
 * x86 named hardware event parsing (from libpfc EVENT_LIST, MIT).
 *
 * Strings passed to ph_set_evt (e.g. cpu_clk_unhalted.core_clk) are parsed into
 * IA32_PERFEVTSEL config words for ph_pmu_wr_cfgs().
 */
#include "ph_pmu_x86.h"
#include <ctype.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

struct ph_umask {
    uint64_t    umask_val;
    const char *name;
};
struct ph_event {
    uint64_t              evt_num;
    const struct ph_umask *umasks;
    const char           *name;
};

static const struct ph_umask UMASK_LIST[]         = {
    {0x02, "store_forward"},        /*   0 */ /* 0x03 */
    {0x08, "no_sr"},
    {0x00, NULL},
    {0x01, "loads"},                /*   3 */ /* 0x05 */
    {0x02, "stores"},
    {0x00, NULL},
    {0x01, "address_alias"},        /*   6 */ /* 0x07 */
    {0x00, NULL},
    {0x01, "miss_causes_a_walk"},   /*   8 */ /* 0x08 */
    {0x02, "walk_completed_4k"},
    {0x04, "walk_completed_2m_4m"},
    {0x0e, "walk_completed"},
    {0x10, "walk_duration"},
    {0x20, "stlb_hit_4k"},
    {0x40, "stlb_hit_2m"},
    {0x60, "stlb_hit"},
    {0x80, "pde_cache_miss"},
    {0x00, NULL},
    {0x03, "recovery_cycles"},      /*  18 */ /* 0x0D */
    {0x00, NULL},
    {0x01, "any"},                  /*  20 */ /* 0x0E */
    {0x10, "flags_merge"},
    {0x20, "slow_lea"},
    {0x40, "single_mul"},
    {0x00, NULL},
    {0x21, "demand_data_rd_miss"},  /*  25 */ /* 0x24 */
    {0x41, "demand_data_rd_hit"},
    {0xE1, "all_demand_data_rd"},
    {0x42, "rfo_hit"},
    {0x22, "rfo_miss"},
    {0xE2, "all_rfo"},
    {0x44, "code_rd_hit"},
    {0x24, "code_rd_miss"},
    {0x27, "all_demand_miss"},
    {0xE7, "all_demand_references"},
    {0xE4, "all_code_rd"},
    {0x50, "l2_pf_hit"},
    {0x30, "l2_pf_miss"},
    {0xF8, "all_pf"},
    {0x3F, "miss"},
    {0xFF, "references"},
    {0x00, NULL},
    {0x50, "wb_hit"},               /*  42 */ /* 0x27 */
    {0x00, NULL},
    {0x4F, "reference"},            /*  44 */ /* 0x2E */
    {0x41, "miss"},
    {0x00, NULL},
    {0x00, "core_clk"},             /*  47 */ /* 0x3C */
    {0x01, "ref_xclk"},
    {0x00, NULL},
    {0x01, "pending"},              /*  50 */ /* 0x48 */
    {0x00, NULL},
    {0x01, "miss_causes_a_walk"},   /*  52 */ /* 0x49 */
    {0x02, "walk_completed_4k"},
    {0x04, "walk_completed_2m_4m"},
    {0x0E, "walk_completed"},
    {0x10, "walk_duration"},
    {0x20, "stlb_hit_4k"},
    {0x40, "stlb_hit_2m"},
    {0x60, "stlb_hit"},
    {0x80, "pde_cache_miss"},
    {0x00, NULL},
    {0x01, "sw_pf"},                /*  62 */ /* 0x4C */
    {0x02, "hw_pf"},
    {0x00, NULL},
    {0x01, "replacement"},          /*  65 */ /* 0x51 */
    {0x00, NULL},
    {0x01, "abort_conflict"},       /*  67 */ /* 0x54 */
    {0x02, "abort_capacity_write"},
    {0x04, "abort_hle_store_to_elided_lock"},
    {0x08, "abort_hle_elision_buffer_not_empty"},
    {0x10, "abort_hle_elision_buffer_mismatch"},
    {0x20, "abort_hle_elision_buffer_unsupported_alignment"},
    {0x40, "hle_elision_buffer_full"},
    {0x00, NULL},
    {0x04, "int_not_eliminated"},   /*  75 */ /* 0x58 */
    {0x08, "simd_not_eliminated"},
    {0x01, "int_not_eliminated"},
    {0x02, "simd_eliminated"},
    {0x00, NULL},
    {0x01, "ring0"},                /*  80 */ /* 0x5C */
    {0x02, "ring123"},
    {0x00, NULL},
    {0x01, "misc1"},                /*  83 */ /* 0x5D */
    {0x02, "misc2"},
    {0x04, "misc3"},
    {0x08, "misc4"},
    {0x10, "misc5"},
    {0x00, NULL},
    {0x01, "empty_cycles"},         /*  89 */ /* 0x5E */
    {0x00, NULL},
    {0x01, "demand_data_rd"},       /*  91 */ /* 0x60 */
    {0x02, "demand_code_rd"},
    {0x04, "demand_rfo"},
    {0x08, "all_data_rd"},
    {0x00, NULL},
    {0x01, "split_lock_uc_lock_duration"}, /*  96 */ /* 0x63 */
    {0x02, "cache_lock_duration"},
    {0x00, NULL},
    {0x02, "empty"},                /*  99 */ /* 0x79 */
    {0x04, "mite_uops"},
    {0x08, "dsb_uops"},
    {0x10, "ms_dsb_uops"},
    {0x20, "ms_mite_uops"},
    {0x30, "ms_uops"},
    {0x18, "all_dsb_cycles_any_uops"},
    {0x18, "all_dsb_cycles_4_uops"},
    {0x24, "all_mite_cycles_any_uops"},
    {0x24, "all_mite_cycles_4_uops"},
    {0x3C, "mite_all_uops"},
    {0x00, NULL},
    {0x02, "misses"},               /* 111 */ /* 0x80 */
    {0x00, NULL},
    {0x01, "miss_causes_a_walk"},   /* 113 */ /* 0x85 */
    {0x02, "walk_completed_4k"},
    {0x04, "walk_completed_2m_4m"},
    {0x0E, "walk_completed"},
    {0x10, "walk_duration"},
    {0x20, "stlb_hit_4k"},
    {0x40, "stlb_hit_2m"},
    {0x60, "stlb_hit"},
    {0x00, NULL},
    {0x01, "lcp"},                  /* 122 */ /* 0x87 */
    {0x04, "iq_full"},
    {0x00, NULL},
    {0x01, "cond"},                 /* 125 */ /* 0x88 */
    {0x02, "direct_jmp"},
    {0x04, "indirect_jmp_non_call_ret"},
    {0x08, "return_near"},
    {0x10, "direct_near_call"},
    {0x20, "indirect_near_call"},
    {0x40, "nontaken"},
    {0x80, "taken"},
    {0xFF, "all_branches"},
    {0x00, NULL},
    {0x01, "cond"},                 /* 135 */ /* 0x89 */
    {0x04, "indirect_jmp_non_call_ret"},
    {0x08, "return_near"},
    {0x10, "direct_near_call"},
    {0x20, "indirect_near_call"},
    {0x40, "nontaken"},
    {0x80, "taken"},
    {0xFF, "all_branches"},
    {0x00, NULL},
    {0x01, "core"},                 /* 144 */ /* 0x9C */
    {0x00, NULL},
    {0x01, "port_0"},               /* 146 */ /* 0xA1 */
    {0x02, "port_1"},
    {0x04, "port_2"},
    {0x08, "port_3"},
    {0x10, "port_4"},
    {0x20, "port_5"},
    {0x40, "port_6"},
    {0x80, "port_7"},
    {0x00, NULL},
    {0x01, "any"},                  /* 155 */ /* 0xA2 */
    {0x04, "rs"},
    {0x08, "sb"},
    {0x10, "rob"},
    {0x00, NULL},
    {0x01, "cycles_l2_pending"},    /* 160 */ /* 0xA3 */
    {0x02, "cycles_ldm_pending"},
    {0x05, "stalls_l2_pending"},
    {0x08, "cycles_l1d_pending"},
    {0x0C, "stalls_l1d_pending"},
    {0x00, NULL},
    {0x01, "uops"},                 /* 166 */ /* 0xA8 */
    {0x00, NULL},
    {0x01, "itlb_flush"},           /* 168 */ /* 0xAE */
    {0x00, NULL},
    {0x01, "demand_data_rd"},       /* 170 */ /* 0xB0 */
    {0x02, "demand_core_rd"},
    {0x04, "demand_rfo"},
    {0x08, "all_data_rd"},
    {0x00, NULL},
    {0x02, "core"},                 /* 175 */ /* 0xB1 */
    {0x00, NULL},
    {0x11, "dtlb_l1"},              /* 177 */ /* 0xBC */
    {0x21, "itlb_l1"},
    {0x12, "dtlb_l2"},
    {0x22, "itlb_l2"},
    {0x14, "dtlb_l3"},
    {0x24, "itlb_l3"},
    {0x18, "dtlb_memory"},
    {0x28, "itlb_memory"},
    {0x00, NULL},
    {0x01, "dtlb_thread"},          /* 186 */ /* 0xBD */
    {0x20, "stlb_any"},
    {0x00, NULL},
    {0x00, "any_p"},                /* 189 */ /* 0xC0 */
    {0x01, "prec_dist"},
    {0x00, NULL},
    {0x08, "avx_to_sse"},           /* 192 */ /* 0xC1 */
    {0x10, "sse_to_avx"},
    {0x40, "any_wb_assist"},
    {0x00, NULL},
    {0x01, "all"},                  /* 196 */ /* 0xC2 */
    {0x02, "retire_slots"},
    {0x00, NULL},
    {0x02, "memory_ordering"},      /* 199 */ /* 0xC3 */
    {0x04, "smc"},
    {0x20, "maskmov"},
    {0x00, NULL},
    {0x00, "all_branches"},         /* 203 */ /* 0xC4 */
    {0x01, "conditional"},
    {0x02, "near_call"},
    {0x04, "all_branches_pebs"},
    {0x08, "near_return"},
    {0x10, "not_taken"},
    {0x20, "near_taken"},
    {0x40, "far_branch"},
    {0x00, NULL},
    {0x00, "all_branches"},         /* 212 */ /* 0xC5 */
    {0x01, "conditional"},
    {0x04, "all_branches_pebs"},
    {0x20, "near_taken"},
    {0x00, NULL},
    {0x01, "start"},                /* 217 */ /* 0xC8 */
    {0x02, "commit"},
    {0x04, "aborted"},
    {0x08, "aborted_mem"},
    {0x10, "aborted_timer"},
    {0x20, "aborted_unfriendly"},
    {0x40, "aborted_memtype"},
    {0x80, "aborted_events"},
    {0x00, NULL},
    {0x01, "start"},                /* 226 */ /* 0xC9 */
    {0x02, "commit"},
    {0x04, "aborted"},
    {0x08, "aborted_mem"},
    {0x10, "aborted_timer"},
    {0x20, "aborted_unfriendly"},
    {0x40, "aborted_memtype"},
    {0x80, "aborted_events"},
    {0x00, NULL},
    {0x02, "x87_output"},           /* 235 */ /* 0xCA */
    {0x04, "x87_input"},
    {0x08, "simd_output"},
    {0x10, "simd_input"},
    {0x1E, "any"},
    {0x00, NULL},
    {0x20, "lbr_inserts"},          /* 241 */ /* 0xCC */
    {0x00, NULL},
    {0x11, "stlb_miss_loads"},      /* 243 */ /* 0xD0 */
    {0x12, "stlb_miss_stores"},
    {0x21, "lock_loads"},
    {0x41, "split_loads"},
    {0x42, "split_stores"},
    {0x81, "all_loads"},
    {0x82, "all_stores"},
    {0x00, NULL},
    {0x01, "l1_hit"},               /* 251 */ /* 0xD1 */
    {0x02, "l2_hit"},
    {0x04, "l3_hit"},
    {0x08, "l1_miss"},
    {0x10, "l2_miss"},
    {0x20, "l3_miss"},
    {0x40, "hit_lfb"},
    {0x00, NULL},
    {0x01, "xsnp_miss"},            /* 259 */ /* 0xD2 */
    {0x02, "xsnp_hit"},
    {0x04, "xsnp_hitm"},
    {0x08, "xsnp_none"},
    {0x00, NULL},
    {0x01, "local_dram"},           /* 264 */ /* 0xD3 */
    {0x00, NULL},
    {0x1F, "any"},                  /* 266 */ /* 0xE6 */
    {0x00, NULL},
    {0x01, "demand_data_rd"},       /* 268 */ /* 0xF0 */
    {0x02, "rfo"},
    {0x04, "code_rd"},
    {0x08, "all_pf"},
    {0x10, "l1d_wb"},
    {0x20, "l2_fill"},
    {0x40, "l2_wb"},
    {0x80, "all_requests"},
    {0x00, NULL},
    {0x01, "i"},                    /* 277 */ /* 0xF1 */
    {0x02, "s"},
    {0x04, "e"},
    {0x07, "all"},
    {0x00, NULL},
    {0x05, "demand_clean"},         /* 282 */ /* 0xF2 */
    {0x06, "demand_dirty"},
    {0x00, NULL}
};
static const struct ph_event EVENT_LIST[256]      = {
    {0x03, &UMASK_LIST[   0], "ld_blocks"},
    {0x05, &UMASK_LIST[   3], "misalign_mem_ref"},
    {0x07, &UMASK_LIST[   6], "ld_blocks_partial"},
    {0x08, &UMASK_LIST[   8], "dtlb_load_misses"},
    {0x0D, &UMASK_LIST[  18], "int_misc"},
    {0x0E, &UMASK_LIST[  20], "uops_issued"},
    {0x24, &UMASK_LIST[  25], "l2_rqsts"},
    {0x27, &UMASK_LIST[  42], "l2_demand_rqsts"},
    {0x2E, &UMASK_LIST[  44], "llc"},
    {0x3C, &UMASK_LIST[  47], "cpu_clk_unhalted"},
    {0x48, &UMASK_LIST[  50], "l1d_pend_miss"},
    {0x49, &UMASK_LIST[  52], "dtlb_store_misses"},
    {0x4C, &UMASK_LIST[  62], "load_hit_pre"},
    {0x51, &UMASK_LIST[  65], "l1d"},
    {0x54, &UMASK_LIST[  67], "tx_mem"},
    {0x58, &UMASK_LIST[  75], "move_elimination"},
    {0x5C, &UMASK_LIST[  80], "cpl_cycles"},
    {0x5D, &UMASK_LIST[  83], "tx_exec"},
    {0x5E, &UMASK_LIST[  89], "rs_events"},
    {0x60, &UMASK_LIST[  91], "offcore_requests_outstanding"},
    {0x63, &UMASK_LIST[  96], "lock_cycles"},
    {0x79, &UMASK_LIST[  99], "idq"},
    {0x80, &UMASK_LIST[ 111], "icache"},
    {0x85, &UMASK_LIST[ 113], "itlb_misses"},
    {0x87, &UMASK_LIST[ 122], "ild_stall"},
    {0x88, &UMASK_LIST[ 125], "br_inst_exec"},
    {0x89, &UMASK_LIST[ 135], "br_misp_exec"},
    {0x9C, &UMASK_LIST[ 144], "idq_uops_not_delivered"},
    {0xA1, &UMASK_LIST[ 146], "uops_executed_port"},
    {0xA2, &UMASK_LIST[ 155], "resource_stalls"},
    {0xA3, &UMASK_LIST[ 160], "cycle_activity"},
    {0xA8, &UMASK_LIST[ 166], "lsd"},
    {0xAE, &UMASK_LIST[ 168], "itlb"},
    {0xB0, &UMASK_LIST[ 170], "offcore_requests"},
    {0xB1, &UMASK_LIST[ 175], "uops_executed"},
    /* Ghost [0xB7] and [0xBB] OFF_CORE_RESPONSE_[01] */
    {0xBC, &UMASK_LIST[ 177], "page_walker_loads"},
    {0xBD, &UMASK_LIST[ 186], "tlb_flush"},
    {0xC0, &UMASK_LIST[ 189], "inst_retired"},
    {0xC1, &UMASK_LIST[ 192], "other_assists"},
    {0xC2, &UMASK_LIST[ 196], "uops_retired"},
    {0xC3, &UMASK_LIST[ 199], "machine_clears"},
    {0xC4, &UMASK_LIST[ 203], "br_inst_retired"},
    {0xC5, &UMASK_LIST[ 212], "br_misp_retired"},
    {0xC8, &UMASK_LIST[ 217], "hle_retired"},
    {0xC9, &UMASK_LIST[ 226], "rtm_retired"},
    {0xCA, &UMASK_LIST[ 235], "fp_assist"},
    {0xCC, &UMASK_LIST[ 241], "rob_misc_events"},
    /* Ghost [0xCD] MEM_TRANS_RETIRED */
    {0xD0, &UMASK_LIST[ 243], "mem_uops_retired"},
    {0xD1, &UMASK_LIST[ 251], "mem_load_uops_retired"},
    {0xD2, &UMASK_LIST[ 259], "mem_load_uops_l3_hit_retired"},
    {0xD3, &UMASK_LIST[ 264], "mem_load_uops_l3_miss_retired"},
    {0xE6, &UMASK_LIST[ 266], "baclears"},
    {0xF0, &UMASK_LIST[ 268], "l2_trans"},
    {0xF1, &UMASK_LIST[ 277], "l2_lines_in"},
    {0xF2, &UMASK_LIST[ 282], "l2_lines_out"},
    {0x00, NULL             , NULL}
};

PH_PMU_CFG
ph_pmu_parse_cfg(const char *s)
{
    uint64_t edge_triggered = 0;
    uint64_t evt_num = 0;
    uint64_t umask_val = 0;
    uint64_t user = 1;
    uint64_t os = 0;
    uint64_t anythread = 0;
    uint64_t inv = 0;
    uint64_t cmask = 0;
    int i = 0;
    int done_mode = 0;
    const struct ph_umask *umask_list = NULL;
    PH_PMU_CFG cfg;

    if (!s || s[0] == '\0')
        return 0;

    if (s[0] == '*') {
        edge_triggered = 1;
        s++;
    }

    i = 0;
    while (EVENT_LIST[i].name) {
        int n = (int)strlen(EVENT_LIST[i].name);
        if (strncasecmp(s, EVENT_LIST[i].name, (size_t)n) == 0 && s[n] == '.') {
            evt_num = EVENT_LIST[i].evt_num;
            umask_list = EVENT_LIST[i].umasks;
            s += n + 1;
            break;
        }
        i++;
    }
    if (!EVENT_LIST[i].name) {
        evt_num = strtoull(s, (char **)&s, 0);
        if (s[0] != '.' || evt_num > 0xFF)
            return 0;
        s++;
        i = 0;
        while (EVENT_LIST[i].name) {
            if (EVENT_LIST[i].evt_num == evt_num) {
                umask_list = EVENT_LIST[i].umasks;
                break;
            }
            i++;
        }
        if (!umask_list)
            return 0;
    }

    i = 0;
    while (umask_list[i].name) {
        int n = (int)strlen(umask_list[i].name);
        if (strncasecmp(s, umask_list[i].name, (size_t)n) == 0 &&
            (s[n] == '\0' || s[n] == '<' || (s[n] == '>' && s[n + 1] == '=') || s[n] == ':')) {
            umask_val = umask_list[i].umask_val;
            s += n;
            break;
        }
        i++;
    }
    if (!umask_list[i].name) {
        umask_val = strtoull(s, (char **)&s, 0);
        if (umask_val > 0xFF || (s[0] != '\0' && s[0] != '<' &&
            !(s[0] == '>' && s[1] == '=') && s[0] != ':'))
            return 0;
    }

    if (s[0] == '>' && s[1] == '=' && isdigit((unsigned char)s[2])) {
        inv = 0;
        cmask = strtoull(s + 2, (char **)&s, 0) & 0xFF;
    } else if (s[0] == '<' && isdigit((unsigned char)s[1])) {
        inv = 1;
        cmask = strtoull(s + 1, (char **)&s, 0) & 0xFF;
    }

    if (s[0] == ':') {
        anythread = user = os = 0;
        while (!done_mode) {
            switch (*++s) {
            case 'A':
            case 'a': anythread = 1; break;
            case 'U':
            case 'u': user = 1; break;
            case 'K':
            case 'k': os = 1; break;
            default: done_mode = 1; break;
            }
        }
    }

    cfg = (cmask << 24) | (inv << 23) | (1ULL << 22) | (anythread << 21) |
          (edge_triggered << 18) | (os << 17) | (user << 16) |
          (umask_val << 8) | evt_num;
    return cfg;
}
