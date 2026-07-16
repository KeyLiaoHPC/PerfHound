#!/bin/bash
# Run an MPI PerfHound example with core binding.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
EX="$(cd "$(dirname "$0")" && pwd)"
BIN="${1:?usage: run_mpi.sh <binary>}"
NP="${NP:-16}"

# Optional HPC Kit MPI (c920fn1 / Arm)
if [ -f /astrum/opt/920f/HPCKit-25.1.RC1/latest/hmpi/gcc/env/setvars.sh ]; then
    # shellcheck disable=SC1091
    source /astrum/opt/920f/HPCKit-25.1.RC1/latest/hmpi/gcc/env/setvars.sh
fi

export LD_LIBRARY_PATH="${ROOT}/src/probe/lib:${LD_LIBRARY_PATH:-}"

cd "$EX"
MPI_OPTS=(--map-by core --bind-to core)
if [ "$(uname -m)" = "aarch64" ]; then
    MPI_OPTS=(--mca btl self,vader "${MPI_OPTS[@]}")
fi
exec mpirun -np "$NP" "${MPI_OPTS[@]}" "./${BIN}"
