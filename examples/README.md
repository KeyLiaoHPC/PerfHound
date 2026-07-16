# PerfHound Examples

Runnable demos for the current `ph_*` / `phmpi_*` API. Build the probe first (`src/probe/make.config`), then compile examples here.

## Prerequisites

```bash
# 1) Configure probe (match PH_EVMODE with example MODE, typically EV)
cd src/probe
# PH_API=PAPI or PHASM, PH_EVMODE=EV, PH_DEBUG=NO
make clean && make && make install

# 2) Build examples (from repo root)
cd examples
make all          # probe + all binaries for this architecture
export LD_LIBRARY_PATH=$PWD/../src/probe/lib:$LD_LIBRARY_PATH
```

Default build: `MODE=EV`, `NP=16`, output under `./ph_data/<name>/`.

Post-process intervals:

```bash
python3 summarize_run.py ./ph_data/test3_asmmpi_x86
```

---

## test1_asm_x86.c (serial, x86-64)

**Purpose:** Serial micro-kernel timing with optional PMU events.

**What it runs:** Repeated dependent ADD (or `KNAME`) inline-asm loop; `ph_read` at Start/End tags (gid=1, pid=1/2).

**Expected output:** `./ph_data/test1_asm_x86/run_<N>/` with one `r0c<cpu>.csv`. Pair Start→End gives `Δcycle`, `Δns`, and `ev1..ev4` in EV mode.

**Run:**

```bash
make test1
./run_serial.sh test1_asm_x86.x 0
python3 summarize_run.py ./ph_data/test1_asm_x86
```

---

## test3_asmmpi_x86.c (MPI, x86-64)

**Purpose:** Multi-rank micro-kernel + small STREAM triad between measurements.

**What it runs:** `mpirun -np 16 --map-by core --bind-to core`; each rank measures asm kernel intervals.

**Expected output:** 16 files `r<rank>c<cpu>.csv` under host directory; `rankmap.csv` maps ranks to CPUs.

**Run:**

```bash
make test3
NP=16 ./run_mpi.sh test3_asmmpi_x86.x
python3 summarize_run.py ./ph_data/test3_asmmpi_x86
```

---

## test5_vardist_add.c (serial, variation)

**Purpose:** Performance variation — random loop counts with cache flushing.

**What it runs:** `NTESTS` iterations (default 200 in Makefile); Gaussian `nins` via libc Box–Muller and `/dev/urandom`.

**Expected output:** `./ph_data/test5_vardist/run_<N>/` with hundreds of Start/End pairs; `test5_rand_list.txt` lists generated loop counts.

**Run:**

```bash
make test5
./run_serial.sh test5_vardist_add.x
python3 summarize_run.py ./ph_data/test5_vardist
```
---

## test6.c (MPI, x86-64, lightweight)

**Purpose:** Minimal MPI example (no STREAM triad); same CSV layout as test3.

**Run:**

```bash
make test6
NP=16 ./run_mpi.sh test6.x
python3 summarize_run.py ./ph_data/test6
```

---

## stream_mpi.c (MPI STREAM + PerfHound)

**Purpose:** STREAM Copy/Scale/Add/Triad with per-kernel `phmpi_read` tags.

**What it runs:** MPI STREAM (`STREAM_ARRAY_SIZE` default 2M elements); tags pid 2–9 mark kernel start/end.

**Expected output:** `./ph_data/stream_mpi/run_<N>/` with per-rank CSV; summarize shows Copy/Scale/Add/Triad intervals per core.

**Run:**

```bash
make stream
NP=16 ./run_mpi.sh stream_mpi.x
python3 summarize_run.py ./ph_data/stream_mpi
```

---

## test2_asm_armv8.c / test4_asmmpi_armv8.c (Arm only)

Built on `aarch64` only (`make test2` / `make test4`). Same API pattern with Arm PMU event names.

---

## Obsolete (not built by default)

- `stream_varapi_selftest.c`, `stream_varapi_selftest_inner.c` — legacy `varapi` / `vt_*` API.

---

## Run all (x86)

```bash
chmod +x run_all.sh run_serial.sh run_mpi.sh
./run_all.sh
```

Variables: `MODE`, `NINS`, `NMEASURE`, `NP`, `KNAME`, `STREAM_ARRAY_SIZE`, `NTIMES`.
