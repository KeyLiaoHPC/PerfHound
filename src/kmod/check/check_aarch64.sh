#!/usr/bin/env bash
# aarch64 post-load check: EL0 PMU register reads work after ph_enable_pmu load.
set -euo pipefail

fail() { echo "*** [PH-kmod] check failed: $*" >&2; exit 1; }

CHECK_SRC=$(mktemp /tmp/ph_pmu_check_XXXXXX.c)
CHECK_BIN=$(mktemp /tmp/ph_pmu_check_XXXXXX)

cat > "$CHECK_SRC" <<'EOF'
#include <stdio.h>
#include <stdint.h>

int main(void) {
    uint64_t pmcr, pmccntr;
    asm volatile("mrs %0, pmcr_el0" : "=r"(pmcr));
    asm volatile("mrs %0, pmccntr_el0" : "=r"(pmccntr));
    if (pmcr == 0 && pmccntr == 0) {
        return 2;
    }
    return 0;
}
EOF

if ! cc -O0 -o "$CHECK_BIN" "$CHECK_SRC" 2>/dev/null; then
    echo "*** [PH-kmod] warning: cannot compile aarch64 PMU probe; skipping EL0 register test" >&2
    rm -f "$CHECK_SRC" "$CHECK_BIN"
    exit 0
fi

if ! "$CHECK_BIN"; then
    rc=$?
    rm -f "$CHECK_SRC" "$CHECK_BIN"
    if [ "$rc" -eq 2 ]; then
        fail "EL0 PMU register read failed; confirm ph_enable_pmu.ko is loaded correctly"
    fi
    fail "PMU probe exited abnormally (rc=${rc})"
fi

rm -f "$CHECK_SRC" "$CHECK_BIN"
echo "*** [PH-kmod] aarch64 EL0 PMU register checks passed"
