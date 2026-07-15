[English](README.md) | [中文](README_CN.md)

# PerfHound

PerfHound is a lightweight toolset for code-level performance profiling, specifically designed for performance analysis of parallel computing applications and performance variation detection on HPC clusters.

For detailed documentation in Chinese and English, please refer to the **docs/** directory.

## 1. Core Features

PerfHound mainly serves the following three goals:
1. **Instrumentation Profiling**: By instrumenting the critical paths of the code, it uses high-precision timestamps (cycle counts, wall-clock time) to characterize the execution time distribution and performance hotspots of code sections.
2. **Performance Counter Sampling**: Synchronously reads hardware PMU (Performance Monitor Unit) events (such as instruction count, cache hit rate, branch prediction, etc.) at the instrumentation points, supporting x86-64 and Armv8-A architectures.
3. **Performance Variation Analysis**: Performs a large number of repeated measurements on the same code section to statistically analyze the execution time distribution, measurement overhead, and the minimum performance variation distinguishable by the system.

**Key Characteristics:**
- **Extremely Low Overhead**: Directly reads underlying registers (such as `rdtsc`/`rdpmc` on x86, `cntvct_el0` on Arm) via the `PHASM` backend, avoiding the extra overhead caused by system calls.
- **Compile-time Static Configuration**: Solidifies the sampling mode (`TS`/`EV`/`EVX`) and buffer size at compile time via `make.config`, minimizing runtime conditional branch judgments.
- **Lock-free Concurrent Writing**: For MPI multi-node and multi-core scenarios, it adopts a strategy of splitting files independently by `(hostname, rank, cpu)`, completely avoiding file lock contention.

## 2. Installation and Compilation

### 2.1 Prepare Kernel Modules (Obtain User-Space PMU Access)

To read hardware performance counters with low latency in user space, build and load the unified kernel module:

```bash
cd src/kmod
make
sudo insmod src/kmod/x86/ph_enable_pmu.ko      # x86-64
# or
sudo insmod src/kmod/aarch64/ph_enable_pmu.ko  # Armv8-A
make check   # fails if module not loaded; run after insmod
```

- **x86-64**: `ph_enable_pmu.ko` enables user-space `rdpmc`, exposes sysfs `config`/`counts` for counter programming, and on load attempts to disable `nmi_watchdog` and unload `iTCO_*` modules.
- **Armv8-A**: `ph_enable_pmu.ko` enables EL0 PMU register access (`PMUSERENR_EL0`). No external libpfc dependency.

Unload: `sudo rmmod ph_enable_pmu`

### 2.2 Configure and Compile PH-Probe API

The core probe of PerfHound supports high customization. Before compiling, please modify the `src/probe/make.config` file:

```makefile
# Example of common configuration
PH_API = PHASM       # Use the underlying PHASM backend (recommended) or PAPI
PH_EVMODE = TS       # Sampling mode: TS (Timestamp only), EV (+4 events), EVX (Extended events)
PH_PARMODE = MPI     # Target program type: SERIAL or MPI
PH_BUFSIZE = 4       # Memory buffer size (in KiB)
PH_DEBUG = NO        # Whether to enable debug logging (set to NO for formal measurements)
```

After configuration, execute the compilation:

```bash
cd src/probe
make clean && make
make install
```
Upon completion, `libph.so` (serial) and `libphmpi.so` (MPI) will be generated under `src/probe/lib`, and public header files will be generated under `src/probe/include`.

### 2.3 Link the Instrumented Program

When compiling your application, include the PerfHound header files and dynamic libraries:

```bash
# Serial program
gcc -O2 -I./src/probe/include -L./src/probe/lib -o myapp myapp.c -lph

# MPI program
mpicc -O2 -I./src/probe/include -L./src/probe/lib -o myapp_mpi myapp.c -lphmpi
```
Before running, make sure to add the directory containing `libph.so` to the `LD_LIBRARY_PATH` environment variable.

### 2.4 Run Functional Tests

From the repository root:

```bash
make check
```

This builds PH-Probe (if needed), probes PAPI (`#include <papi.h>` + `-lpapi`) and MPI
(`#include <mpi.h>` + `-lmpi`), then runs serial unit/integration tests (`A*`, `B*`) and
MPI tests (`AM*`, `BM*`) when MPI is available. See [`tests/README.md`](tests/README.md)
for per-test pass/fail criteria.


The PerfHound API has a strict calling sequence: `init` -> `set_tag` -> `set_evt` -> `commit` -> `read` -> `finalize`.

### 3.1 Serial Program Example

```c
#include <perfhound.h>

int main() {
    // 1. Initialize and specify the data output directory
    ph_init("./ph_data");

    // 2. Register instrumentation point tags (gid >= 1)
    ph_set_tag(1, 1, "Compute_Start");
    ph_set_tag(1, 2, "Compute_End");

    // 3. Register performance events (valid only in EV/EVX mode, must be called before commit)
    ph_set_evt("CPU_CYCLES");
    ph_set_evt("INST_RETIRED");

    // 4. Commit configuration
    ph_commit();

    // 5. Sample in the critical path (hot path)
    for (int i = 0; i < 1000; i++) {
        ph_read(1, 1, 0.0);
        // ... your core computation code ...
        ph_read(1, 2, 0.0);
    }

    // 6. Finalize and flush to disk
    ph_finalize();
    return 0;
}
```

### 3.2 MPI Program Example

The usage for MPI programs is similar, just change the prefix to `phmpi_`, and ensure they are called after `MPI_Init` and before `MPI_Finalize`:

```c
#include <mpi.h>
#include <ph_mpi.h>

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    phmpi_init("./ph_data");
    phmpi_set_tag(1, 1, "MPI_Work_Start");
    phmpi_set_tag(1, 2, "MPI_Work_End");
    phmpi_commit(); // Internally includes MPI_Barrier synchronization

    phmpi_read(1, 1, 0.0);
    // ... computation and communication ...
    phmpi_read(1, 2, 0.0);

    phmpi_finalize();
    MPI_Finalize();
    return 0;
}
```

## 4. Output Data Structure

After the run finishes, the data will be saved in the directory specified by `ph_init` (e.g., `./ph_data`), and a `run_<N>` folder will be automatically generated with an incremented number for each run:

```text
ph_data/
├── run_info.csv              # Global run metadata
└── run_1/
    ├── ctag.csv              # Instrumentation point tag dictionary
    ├── etag.csv              # Performance event dictionary
    ├── rankmap.csv           # Mapping from Rank to physical node/CPU
    └── node01.example.com/   # Directory categorized by hostname
        ├── r0c0.csv          # Sampling data for Rank 0, CPU 0
        └── r1c1.csv          # Sampling data for Rank 1, CPU 1
```

For detailed descriptions of the output fields, please refer to [`docs/output_file_CN.md`](docs/output_file_CN.md). The internally used GID 0 markers (such as `PH_S_main`, `PH_S_ph_dump`) are used to record the probe's own overhead and lifecycle.

## 5. Examples and Tools

You can use the code in the `examples/` directory to reproduce the cases on your own computing platform, so as to quickly get started with PerfHound:
- `test2_asm_armv8.c` / `test1_asm_x86.c`: Serial instrumentation and PMU sampling examples
- `test4_asmmpi_armv8.c` / `test3_asmmpi_x86.c`: MPI parallel instrumentation examples
- `test5_vardist_add.c`: Performance variation distribution data collection example

For post-processing scripts and multi-level detection workflows for variation analysis, please refer to `src/resolution_test/` and `src/varstat/`.

## 6. Current Limitations and Development Plan

- Dynamically changing the binding relationship between processes and CPU cores during runtime is not supported.
- OpenMP thread-level profiling is not supported yet.
- The Armv8-A kernel module (`ph_enable_pmu.ko`) may not unload cleanly in some environments.
- An explicit `ph_dump` interface is retained, and the API calling sequence is strictly required; incorrect sequence may cause runtime exceptions.
- Post-processing and statistical analysis tools (`ph_vis`, `ph_stat`) are still being refined.
