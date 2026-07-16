#!/bin/bash
# Run an MPI PerfHound example with core binding.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
EX="$(cd "$(dirname "$0")" && pwd)"
BIN="${1:?usage: run_mpi.sh <binary>}"
NP="${NP:-16}"

export LD_LIBRARY_PATH="${ROOT}/src/probe/lib:${LD_LIBRARY_PATH:-}"

cd "$EX"
exec mpirun -np "$NP" --map-by core --bind-to core "./${BIN}"
