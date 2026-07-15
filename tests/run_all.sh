#!/usr/bin/env bash
# Run PerfHound functional tests and summarize PASS/FAIL/SKIP.
set -u

BIN_DIR="${1:-.}"
MPI_LAUNCHER="${2:-}"
NP="${3:-2}"
PROBE_API="${PROBE_API:-PHASM}"
HAVE_KMOD="${HAVE_KMOD:-no}"

passed=0
failed=0
skipped=0

if [[ "$PROBE_API" == "PHASM" && "$HAVE_KMOD" != "yes" ]]; then
    echo "*** [PH-tests] SKIP all: ph_enable_pmu kernel module not loaded (required for PHASM)"
    echo "*** [PH-tests] Load with: cd src/kmod && sudo insmod <arch>/ph_enable_pmu.ko"
    skipped=15
    echo "*** [PH-tests] 0 passed, 0 failed, $skipped skipped"
    exit 0
fi

run_one() {
    local id="$1"
    local bin="$2"
    local use_mpi="$3"
    local rc=0

    if [[ ! -x "$bin" ]]; then
        echo "*** [PH-tests] [$id] SKIP: binary not built ($bin)"
        skipped=$((skipped + 1))
        return 0
    fi

    if [[ "$use_mpi" == "mpi" ]]; then
        if [[ -z "$MPI_LAUNCHER" ]]; then
            echo "*** [PH-tests] [$id] SKIP: MPI launcher not found"
            skipped=$((skipped + 1))
            return 0
        fi
        "$MPI_LAUNCHER" -np "$NP" "$bin"
        rc=$?
    else
        "$bin"
        rc=$?
    fi

    case "$rc" in
        0)
            passed=$((passed + 1))
            ;;
        2)
            skipped=$((skipped + 1))
            ;;
        *)
            failed=$((failed + 1))
            echo "*** [PH-tests] [$id] FAIL: exit code $rc"
            ;;
    esac
}

should_run() {
    local id="$1"
    if [[ -z "${TESTS:-}" ]]; then
        return 0
    fi
    [[ ",${TESTS}," == *",${id},"* ]]
}

SERIAL_IDS=(A1 A2 A3 A4 A5 A6 B1 B2 B3)
MPI_IDS=(AM1 AM2 AM3 AM4 BM1 BM2)

for id in "${SERIAL_IDS[@]}"; do
    if should_run "$id"; then
        run_one "$id" "$BIN_DIR/$id" serial
    fi
done

for id in "${MPI_IDS[@]}"; do
    if should_run "$id"; then
        run_one "$id" "$BIN_DIR/$id" mpi
    fi
done

echo "*** [PH-tests] $passed passed, $failed failed, $skipped skipped"
if [[ "$failed" -gt 0 ]]; then
    exit 1
fi
exit 0
