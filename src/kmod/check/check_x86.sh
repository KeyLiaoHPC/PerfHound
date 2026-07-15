#!/usr/bin/env bash
# x86 post-load checks: sysfs nodes, CR4.PCE, nmi_watchdog.
set -euo pipefail

MOD_SYS="/sys/module/ph_enable_pmu"
fail() { echo "*** [PH-kmod] check failed: $*" >&2; exit 1; }

for node in config counts masks cr4.pce; do
    [ -e "${MOD_SYS}/${node}" ] || fail "missing sysfs node ${MOD_SYS}/${node}"
done

PCE=$(cat "${MOD_SYS}/cr4.pce" 2>/dev/null | tr -d '\n')
[ "$PCE" = "1" ] || fail "CR4.PCE not enabled (cr4.pce=${PCE}); user-space rdpmc unavailable"

if [ -r /proc/sys/kernel/nmi_watchdog ]; then
    NMI=$(cat /proc/sys/kernel/nmi_watchdog | tr -d '\n')
    [ "$NMI" = "0" ] || fail "nmi_watchdog is still ${NMI}; fixed counters may be in use"
fi

test -r "${MOD_SYS}/config"
dd if="${MOD_SYS}/config" of=/dev/null bs=8 count=1 status=none 2>/dev/null \
    || fail "cannot read ${MOD_SYS}/config"

echo "*** [PH-kmod] x86 sysfs / watchdog checks passed"
