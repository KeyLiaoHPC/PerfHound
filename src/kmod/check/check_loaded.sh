#!/usr/bin/env bash
# Step 1 of make check: ph_enable_pmu must already be loaded (no auto-insmod).
set -euo pipefail

ARCH="${1:-unknown}"

if [ -d /sys/module/ph_enable_pmu ]; then
    exit 0
fi

if lsmod 2>/dev/null | awk '{print $1}' | grep -qx ph_enable_pmu; then
    exit 0
fi

echo "*** [PH-kmod] check failed: kernel module ph_enable_pmu is not loaded." >&2
echo "*** [PH-kmod] Load it as root first:" >&2
echo "*** [PH-kmod]   sudo insmod src/kmod/${ARCH}/ph_enable_pmu.ko" >&2
echo "*** [PH-kmod] Then run: cd src/kmod && make check" >&2
exit 1
