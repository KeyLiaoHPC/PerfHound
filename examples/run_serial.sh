#!/bin/bash
# Run a serial PerfHound example with optional CPU binding.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
EX="$(cd "$(dirname "$0")" && pwd)"
BIN="${1:?usage: run_serial.sh <binary> [cpu_id]}"

export LD_LIBRARY_PATH="${ROOT}/src/probe/lib:${LD_LIBRARY_PATH:-}"

cd "$EX"
CPU="${2:-0}"
if command -v taskset >/dev/null 2>&1; then
    exec taskset -c "$CPU" "./${BIN}"
else
    exec "./${BIN}"
fi
