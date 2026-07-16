#!/bin/bash
# Build probe + all x86 examples, run them, and summarize CSV output.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
EX="$(cd "$(dirname "$0")" && pwd)"
NP="${NP:-16}"

source /home/hpckey/01-App/01-Script/env_vihps.sh 2>/dev/null || true
export LD_LIBRARY_PATH="${ROOT}/src/probe/lib:${LD_LIBRARY_PATH:-}"

cd "$EX"
chmod +x run_serial.sh run_mpi.sh summarize_run.py

make probe all-x86

./run_serial.sh test1_asm_x86.x
python3 summarize_run.py ./ph_data/test1_asm_x86 | head -20

NP="$NP" ./run_mpi.sh test3_asmmpi_x86.x
python3 summarize_run.py ./ph_data/test3_asmmpi_x86 | head -40

./run_serial.sh test5_vardist_add.x
python3 summarize_run.py ./ph_data/test5_vardist | head -20

NP="$NP" ./run_mpi.sh test6.x
python3 summarize_run.py ./ph_data/test6 | head -40

NP="$NP" ./run_mpi.sh stream_mpi.x
python3 summarize_run.py ./ph_data/stream_mpi | head -80

echo "All example runs completed."
