#!/bin/bash
# Legacy wrapper — use: make test3 NINS=100 MODE=EV
set -euo pipefail
cd "$(dirname "$0")"
NINS="${1:-100}"
make test3 NINS="$NINS" MODE="${MODE:-EV}" KNAME="${KNAME:-ADD}"
