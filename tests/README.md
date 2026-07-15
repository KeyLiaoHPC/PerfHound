# PerfHound Functional Tests

This directory contains the PH-Probe functional test suite. Tests are grouped into
**unit tests** (single API surface) and **integration tests** (instrumented
mini-programs). Run the full suite from the repository root:

```bash
make check
```

Run a subset:

```bash
make -C tests check TESTS=A2,B1
```

## Environment detection

Before building tests, `tests/Makefile` probes the toolchain:

| Variable | Detection | Effect |
|----------|-----------|--------|
| `HAVE_PAPI` | `#include <papi.h>` **and** link `-lpapi` both succeed | `yes` → build probe with `PH_API=PAPI`; `no` → use `PHASM` for tests |
| `HAVE_MPI` | `#include <mpi.h>` **and** link `-lmpi` both succeed | `yes` → build/run `AM*` and `BM*`; `no` → skip MPI tests |

MPI runtime additionally requires `mpirun` or `mpiexec`. If the MPI library is
present but no launcher is found, MPI tests are **SKIP** (exit code 2), not FAIL.

| `HAVE_KMOD` | `/sys/module/ph_enable_pmu` exists | Required when `PROBE_API=PHASM`; otherwise all tests **SKIP** |

Exit codes: `0` = PASS, `2` = SKIP, other = FAIL.

When using PHASM (no PAPI), load the kernel module first:

```bash
cd src/kmod && sudo insmod x86/ph_enable_pmu.ko   # or aarch64/
```

## Numbering

| Prefix | Type | Description |
|--------|------|-------------|
| `A*` | Unit (serial) | One or few `ph_*` APIs per test |
| `AM*` | Unit (MPI) | `phmpi_*` and MPI boundaries |
| `B*` | Integration (serial) | Simulated workload with full instrumentation |
| `BM*` | Integration (MPI) | Multi-rank simulated workload |

## A* — Serial unit tests

### A1 — Init / Finalize

| Field | Detail |
|-------|--------|
| Source | `unit/a1_init.c` |
| Content | `ph_init` → `ph_commit` → minimal `ph_read` → `ph_finalize` |
| **PASS** | `ph_init` returns 0; temp output root exists; `run_<N>/` created |
| **FAIL** | `ph_init` non-zero; no output tree; crash |
| **SKIP** | — |

### A2 — Timing read

| Field | Detail |
|-------|--------|
| Source | `unit/a2_timing.c` |
| Content | Multiple `ph_read` samples; inspect record CSV timestamps |
| **PASS** | ≥2 user samples; `cycle`/`nanosec` monotonic and not all zero; GID0 `(0,1)` and `(0,2)` present |
| **FAIL** | No records; timestamps decrease or are all zero; missing internal markers |
| **SKIP** | — |

### A3 — Tags

| Field | Detail |
|-------|--------|
| Source | `unit/a3_tags.c` |
| Content | `ph_set_tag` for user `(gid,pid)` then sample |
| **PASS** | Row in `ctag.csv`; matching `(gid,pid)` in record CSV |
| **FAIL** | Missing ctag row or record sample |
| **SKIP** | — |

### A4 — Counters / set_evt

| Field | Detail |
|-------|--------|
| Source | `unit/a4_counters.c` |
| Content | EV-mode `libph_ev.so`; `ph_set_evt`; post-commit guard |
| **PASS** | Event registered; `etag.csv` non-empty; `ph_set_evt` after `commit` fails; record has `ev1` column |
| **FAIL** | Pre-commit `set_evt` fails (non-env); empty etag; post-commit `set_evt` succeeds |
| **SKIP** | EV library not built; no supported event name; record has no event columns |

### A5 — IO dump tree

| Field | Detail |
|-------|--------|
| Source | `unit/a5_io.c` |
| Content | Directory layout; `ph_dump` flush |
| **PASS** | `run_<N>/`, `ctag.csv`, `rankmap.csv`, host `r0c*.csv` exist; `ph_dump` increases record line count |
| **FAIL** | Missing paths; dump does not append data |
| **SKIP** | — |

### A6 — API order guard

| Field | Detail |
|-------|--------|
| Source | `unit/a6_api_order.c` |
| Content | Valid call sequence; `ph_set_evt` after `commit` |
| **PASS** | Legal sequence completes; post-commit `set_evt` returns non-zero |
| **FAIL** | Legal sequence fails; post-commit `set_evt` returns 0 |
| **SKIP** | — |

## AM* — MPI unit tests (`HAVE_MPI=yes`)

### AM1 — MPI init path

| Field | Detail |
|-------|--------|
| Source | `unit/am1_mpi_init.c` |
| Content | `MPI_Init` → `phmpi_init` → `commit` → `finalize` (2 ranks) |
| **PASS** | All ranks succeed; finishes within launcher timeout |
| **FAIL** | Hang, non-zero exit, crash |
| **SKIP** | MPI library not detected; no `mpirun`/`mpiexec` |

### AM2 — MPI tags barrier

| Field | Detail |
|-------|--------|
| Source | `unit/am2_mpi_tags.c` |
| Content | Each rank calls `phmpi_set_tag`; verify `ctag.csv` |
| **PASS** | Tags written; all ranks pass barrier and finalize |
| **FAIL** | Deadlock; missing user tag in `ctag.csv` |
| **SKIP** | Same as AM1 |

### AM3 — MPI per-rank IO

| Field | Detail |
|-------|--------|
| Source | `unit/am3_mpi_io.c` |
| Content | Per-rank `phmpi_read` and `phmpi_dump` |
| **PASS** | `r0c*.csv` and `r1c*.csv` exist; `rankmap.csv` has ≥ nprocs data rows |
| **FAIL** | Missing rank record file; insufficient rankmap rows |
| **SKIP** | Same as AM1 |

### AM4 — MPI fastread

| Field | Detail |
|-------|--------|
| Source | `unit/am4_mpi_fastread.c` |
| Content | `phmpi_fastread` samples without auto-dump |
| **PASS** | Samples with tag `(3,1)` appear in record; clean finalize |
| **FAIL** | No samples or crash |
| **SKIP** | Same as AM1 |

## B* — Serial integration tests

### B1 — Instrumented busy loop

| Field | Detail |
|-------|--------|
| Source | `integ/b1_busy_loop.c` |
| Content | Loop with start/end `ph_read` pair and `uval` payload |
| **PASS** | Start/end pair present; positive `Δcycle`/`Δnanosec`; `uval` matches iteration count |
| **FAIL** | Missing pair; zero or negative delta; wrong `uval` |
| **SKIP** | — |

### B2 — Instrumented work + events

| Field | Detail |
|-------|--------|
| Source | `integ/b2_work_events.c` |
| Content | EV library; idle vs busy phases with hardware event column |
| **PASS** | `etag.csv` populated; event counter changes across run |
| **FAIL** | EV build available but event delta is zero |
| **SKIP** | Same conditions as A4 |

### B3 — Multi-tag phases

| Field | Detail |
|-------|--------|
| Source | `integ/b3_multi_phase.c` |
| Content | Init / compute / epilogue phases with distinct tags |
| **PASS** | All phase tags in `ctag.csv`; each phase has start sample in record |
| **FAIL** | Missing tag or phase sample |
| **SKIP** | — |

## BM* — MPI integration tests (`HAVE_MPI=yes`)

### BM1 — Dual-rank instrumented loop

| Field | Detail |
|-------|--------|
| Source | `integ/bm1_mpi_loop.c` |
| Content | Each rank runs instrumented busy loop |
| **PASS** | Both ranks produce positive timing deltas; no hang |
| **FAIL** | Missing rank data; unreasonable timing |
| **SKIP** | Same as AM1 |

### BM2 — Rank-local phases

| Field | Detail |
|-------|--------|
| Source | `integ/bm2_mpi_phases.c` |
| Content | Per-rank phase tags and rankmap consistency |
| **PASS** | `rankmap` complete; rank-specific tags in `ctag.csv`; rank 1 record has expected sample |
| **FAIL** | Incomplete rankmap or mismatched tags/records |
| **SKIP** | Same as AM1 |

## Layout

```text
tests/
  Makefile           # build + check orchestration
  run_all.sh         # execute binaries, summarize results
  README.md          # this file
  common/            # ph_test.h, ph_test_util.c
  unit/              # A*, AM* sources
  integ/             # B*, BM* sources
  bin/               # built test executables (gitignored)
  lib/               # temporary EV-mode libph_ev.so copy
```

## Notes

- Tests use only public headers `perfhound.h` / `ph_mpi.h`.
- `make check` restores the default probe build in `src/probe/` after running.
- Kernel module verification remains separate: `cd src/kmod && make check`.
