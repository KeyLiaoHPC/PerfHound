# PerfHound Agent Guide

Applies to all AI-assisted contributions. Breaching these rules can result in automatic banning.

Basic scope: code-level profiling via PH-Probe instrumentation (serial and MPI), plus platform PMU enablement (x86 libpfc / Armv8 kmod) and supporting analysis utilities.

## Must Follow

1. **Style**: Read and follow [`docs/STYLE_GUIDE.md`](docs/STYLE_GUIDE.md) before coding.
2. **Probe edits**: Prefer changing [`src/probe/`](src/probe/) carefully; keep serial (`perfhound.c`) and MPI (`ph_mpi.c`) behavior aligned unless the user asks for a serial-only or MPI-only change.
3. **Public API boundary**: Instrumented apps and examples may include only [`src/probe/include/perfhound.h`](src/probe/include/perfhound.h) (serial) or [`src/probe/include/ph_mpi.h`](src/probe/include/ph_mpi.h) (MPI) — not `ph_core.h`, `ph_pm_*.h`, or other internal headers.
4. **Docs**: User-facing workflow changes (build modes, API call order, data layout, kmod steps) require updates to [`docs/user_manual.md`](docs/user_manual.md) and/or [`README.md`](README.md) with runnable examples.
5. **Examples / tests**: Prefer validating probe changes against [`examples/`](examples/) or [`src/resolution_test/`](src/resolution_test/); ask the user which fixture to use when unclear.

## Build And Verify

Run from workspace root unless noted. Probe build is Makefile-based under `src/probe/` (not the root `Makefile`, which is leftover and unused).

```bash
# 1) Configure probe modes in src/probe/make.config
#    PH_API=PHASM|PAPI   PH_EVMODE=TS|EV|EVX   PH_PARMODE=SERIAL|MPI
#    PH_BUFSIZE=<KiB>    PH_DEBUG=YES|NO

cd src/probe
make clean && make
make install   # installs libph.so / libphmpi.so + perfhound.h under PREFIX (default .)
```

Expected outputs under `src/probe/` (or `PREFIX`):

- `lib/libph.so` — serial probe
- `lib/libphmpi.so` — MPI probe
- `include/perfhound.h` — public serial header

Link an instrumented binary (example sketch):

```bash
# Serial
gcc -O2 -fno-builtin -I./src/probe/include -L./src/probe/lib \
    -o myapp.x myapp.c -lph

# MPI
mpicc -O2 -fno-builtin -I./src/probe/include -L./src/probe/lib \
    -o myapp_mpi.x myapp.c -lphmpi
```

Typical API call order (must keep this sequence):

```c
ph_init(path);                 /* or phmpi_init for MPI */
ph_set_tag(gid, pid, name);    /* optional labels */
ph_set_evt("event_name");      /* EV/EVX only; before commit */
ph_commit();
ph_read(gid, pid, uval);       /* hot path */
ph_dump();                     /* optional force flush */
ph_finalize();
```

Optional platform checks:

```bash
# x86: build/load libpfc, disable nmi_watchdog, then run a short instrumented example
# Armv8: build/load enable_pmu.ko from src/kmod/armv8a/ (or src/pmu/)
```

Data check after a successful run: look under the path passed to `ph_init` (default `./ph_data`) for `run_<N>/`, `ctag.csv`, `etag.csv`, host dirs, and `r<rank>c<cpu>.csv`.

## Code Map

| Area            | Path                                                | Role                                                   |
| --------------- | --------------------------------------------------- | ------------------------------------------------------ |
| Public API      | `src/probe/include/perfhound.h`                   | Serial probe API (`ph_*`)                            |
| MPI public API  | `src/probe/include/ph_mpi.h`                      | MPI probe API (`phmpi_*`)                            |
| Probe core      | `src/probe/perfhound.c`                           | Serial init / read / dump / finalize                   |
| Probe MPI       | `src/probe/ph_mpi.c`, `mpi_op.c`                | MPI init, collectives helpers, MPI read path           |
| Probe I/O       | `src/probe/ph_io.c`                               | Directory tree, CSV writers, run metadata              |
| Internal types  | `src/probe/include/ph_core.h`                     | `rec_t`, buffer sizes, event-mode macros             |
| PMU backends    | `src/probe/include/ph_pm_{x86_64,aarch64,papi}.h` | Low-level`_ph_*` read/config macros                  |
| Event tables    | `src/probe/include/ph_evt_*.h`                    | Named event → code maps                               |
| Probe build     | `src/probe/Makefile`, `make.config`             | Compile-time modes and install                         |
| Arm PMU kmod    | `src/kmod/armv8a/`, `src/pmu/`                  | Enable user-space PMU access on Armv8                  |
| Examples        | `examples/`                                       | Asm / STREAM / MPI instrumentation demos               |
| Resolution test | `src/resolution_test/`                            | Multi-level variation resolution measurement           |
| Utils           | `src/utils/`                                      | Interval→CSV, PMU sanity checks, timers               |
| Stats (Python)  | `src/varstat/`                                    | Post-processing helpers                                |
| RDMA (exp.)     | `src/rdma/`                                       | Experimental RDMA client/server                        |
| User docs       | `docs/user_manual.md`, `README.md`              | Manuals                                                |
| Platform Make   | `setup/Make.*`                                    | Compiler/MPI flag snippets (legacy / platform presets) |

Supporting / rarely touched: root `Makefile` (TPBench leftover — do not use), empty `src/rdma/Makefile`, stale `examples/Makefile` targets that still mention old `varapi` / `pfhprobe` paths.

## Common Change Recipes

### New / changed `make.config` option

- Edit [`src/probe/make.config`](src/probe/make.config) and wire the flag in [`src/probe/Makefile`](src/probe/Makefile) (`CFLAGS += -DPH_OPT_...`).
- Document the option and valid values in [`docs/user_manual.md`](docs/user_manual.md).
- Rebuild: `cd src/probe && make clean && make`.

### Probe API change (serial + MPI)

1. Update prototypes in `perfhound.h` and/or `ph_mpi.h`.
2. Implement in `perfhound.c` and mirror in `ph_mpi.c` when behavior is shared.
3. Keep I/O side effects in `ph_io.c` (`ph_io_*`).
4. Update examples that call the API and the user manual call-order section.
5. Note: some older examples still use the legacy `pfh_*` names — prefer `ph_*` / `phmpi_*` for new code.

### New PMU backend or timer path

1. Add or extend `src/probe/include/ph_pm_<arch>.h` with `_ph_init_*`, `_ph_read_cy`, `_ph_read_ns`, `_ph_parse_event`, `_ph_config_event`, `_ph_read_pm_*`.
2. Include it from `ph_core.h` under the correct arch / `PH_OPT_PAPI` branch.
3. Do not expose backend macros through public headers.
4. Verify with a short `ph_read` loop under TS and EV/EVX as applicable.

### New named hardware event

1. Add an entry to the appropriate `ph_evt_*.h` table.
2. Ensure `_ph_parse_event` can resolve the string.
3. Document the event name string users pass to `ph_set_evt`.

### New utility under `src/utils/`

1. Add a standalone `.c` with a short file header and a compile comment at the top (see existing utils).
2. Keep utils independent of probe internals unless they only consume CSV outputs.
3. Document invocation in `docs/user_manual.md` if user-facing.

### New example

1. Add `examples/<name>.c` that includes only the public header.
2. Follow the init → set_tag/set_evt → commit → read → finalize order.
3. Provide a compile one-liner in a comment or update `examples/compile.sh` / `examples/README.md`.

### Kernel module (Armv8)

1. Edit `src/kmod/armv8a/enable_pmu.c` (prefer this tree over the duplicate under `src/pmu/` unless the user specifies otherwise).
2. Build with the in-tree `Makefile` against the running kernel headers.
3. Document load/unload and any `sysctl` / blacklist steps in the user manual.

## Probe Layout (PH-Probe)

**Build modes** (compile-time, from `make.config`):

| Knob           | Values                  | Effect                                            |
| -------------- | ----------------------- | ------------------------------------------------- |
| `PH_API`     | `PHASM`, `PAPI`     | Direct PMU asm vs PAPI backend                    |
| `PH_EVMODE`  | `TS`, `EV`, `EVX` | Timestamp-only / +4 events / extended event count |
| `PH_PARMODE` | `SERIAL`, `MPI`     | Selects which sources the Makefile emphasizes     |
| `PH_BUFSIZE` | KiB integer             | In-memory record buffer before auto-`ph_dump`   |
| `PH_DEBUG`   | `YES`, `NO`         | Extra logging (hurts instrumentation accuracy)    |

**Libraries**:

| Artifact        | Sources                                 | Role                   |
| --------------- | --------------------------------------- | ---------------------- |
| `libph.so`    | `perfhound.c`, `ph_io.c`            | Serial instrumentation |
| `libphmpi.so` | `ph_mpi.c`, `ph_io.c`, `mpi_op.c` | MPI instrumentation    |

**Record layout** (`rec_t` in `ph_core.h`): `ctag[2]`, cycle, wall-ns, user `double`, optional event counters (`PH_OPT_EV` / `PH_OPT_EVX`).

**Output tree** (under `ph_init` path):

```text
<proj>/
  run_info.csv
  run_<N>/
    ctag.csv
    etag.csv
    rankmap.csv
    <hostname>/
      r<rank>c<cpu>.csv
```

**GID 0** is reserved for probe-internal markers (start, dump begin/end, finalize). User tags should use `gid >= 1`.

## Error Handling Notes

Current probe code typically:

- Returns `int` status from setup helpers (`ph_init`, `ph_set_evt`, `ph_set_tag`, `ph_io_*`).
- Prints `*** [PH-Probe] ...` diagnostics and may `exit(1)` on fatal setup/I/O failures.
- Keeps `ph_read` / `ph_fastread` on the hot path free of heavyweight logging.

When editing:

- Prefer returning errors from new setup/I/O helpers instead of adding new `exit()` calls when practical.
- Do not add noisy prints inside `ph_read` / `ph_fastread`.
- Keep MPI and serial failure messages consistent in wording.
