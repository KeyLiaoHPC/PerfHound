#!/bin/bash
# Legacy wrapper — use: NP=16 make run-test3
set -euo pipefail
cd "$(dirname "$0")"
NP="${NP:-16}" make run-test3
