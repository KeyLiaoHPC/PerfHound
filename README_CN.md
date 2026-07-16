[English](README.md) | [中文](README_CN.md)

# 盘瓠（PerfHound）

盘瓠（PerfHound）是一个轻量级的代码级性能分析（Profiling）工具套件，专为并行计算代码的性能分析和超算集群的性能波动探测而设计。

PerfHound is a toolset for code-level profiling. It is an ideal tool for performance analysis of parallel applications and performance variation detection on HPC Clusters.

详细的中文与英文文档请参阅 **docs/** 目录。

## 1. 核心功能与特性

PerfHound 主要服务于以下三大目标：
1. **插桩性能分析**：通过在代码关键路径打点，利用高精度时间戳（周期数、墙钟时间）刻画代码区间的耗时分布与性能热点。
2. **性能计数器采样**：在打点处同步读取硬件 PMU（Performance Monitor Unit）事件（如指令数、缓存命中率、分支预测等），支持 x86-64 和 Armv8-A 架构。
3. **性能波动分析**：对同一代码区间进行大量重复测量，统计耗时分布、测量开销以及系统可分辨的最小性能波动。

**主要特性：**
- **极低开销**：通过 `PHASM` 后端直接读取底层寄存器（如 x86 的 `rdtsc`/`rdpmc`，Arm 的 `cntvct_el0`），避免系统调用带来的额外开销。
- **编译期静态配置**：通过 `make.config` 在编译期固化采样模式（`TS`/`EV`/`EVX`）与缓冲大小，最大程度减少运行时的条件分支判断。
- **无锁并发写入**：针对 MPI 多节点多核场景，采用按 `(hostname, rank, cpu)` 独立拆分文件的策略，彻底避免文件锁竞争。

## 2. 安装与编译

### 2.1 准备内核模块（获取用户态 PMU 访问权限）

为了在用户态低延迟地读取硬件性能计数器，需编译并加载统一命名的内核模块：

```bash
cd src/kmod
make
sudo insmod src/kmod/x86/ph_enable_pmu.ko      # x86-64
# 或
sudo insmod src/kmod/aarch64/ph_enable_pmu.ko  # Armv8-A
make check   # 若模块未加载会报错退出；insmod 后再运行
```

- **x86-64**：`ph_enable_pmu.ko` 开启用户态 `rdpmc`，通过 sysfs 配置计数器；加载时会尝试关闭 `nmi_watchdog` 并卸载 `iTCO_*` 模块。
- **Armv8-A**：`ph_enable_pmu.ko` 打开 EL0 PMU 寄存器访问权限，不再依赖外部 libpfc。

卸载：`sudo rmmod ph_enable_pmu`

### 2.2 配置并编译 PH-Probe API

PerfHound 的核心探针（Probe）支持高度定制化。在编译前，请修改 `src/probe/make.config` 文件：

```makefile
# 常用配置示例
PH_API = PHASM       # 使用底层的 PHASM 后端（推荐）或 PAPI
PH_EVMODE = TS       # 采样模式：TS (仅时间戳), EV (加4个事件), EVX (扩展事件)
PH_PARMODE = MPI     # 目标程序类型：SERIAL 或 MPI
PH_BUFSIZE = 4       # 内存缓冲区大小（单位：KiB）
PH_DEBUG = NO        # 是否开启调试日志（正式测量请设为 NO）
```

配置完成后，执行编译：

```bash
cd src/probe
make clean && make
make install
```
完成后，会在 `src/probe/lib` 下生成 `libph.so`（串行）和 `libphmpi.so`（MPI），并在 `src/probe/include` 下生成公共头文件。

### 2.3 链接被测程序

在编译你的应用程序时，引入 PerfHound 的头文件和动态库：

```bash
# 串行程序
gcc -O2 -I./src/probe/include -L./src/probe/lib -o myapp myapp.c -lph

# MPI 程序
mpicc -O2 -I./src/probe/include -L./src/probe/lib -o myapp_mpi myapp.c -lphmpi
```
运行前，请确保将 `libph.so` 所在的目录加入到 `LD_LIBRARY_PATH` 环境变量中。

## 3. API 使用指南

PerfHound API 具有严格的调用顺序：`init` -> `set_tag` -> `set_evt` -> `commit` -> `read` -> `finalize`。

### 3.1 串行程序示例

```c
#include <perfhound.h>

int main() {
    // 1. 初始化并指定数据输出目录
    ph_init("./ph_data");

    // 2. 注册插桩点标签 (gid >= 1)
    ph_set_tag(1, 1, "Compute_Start");
    ph_set_tag(1, 2, "Compute_End");

    // 3. 注册性能事件 (仅在 EV/EVX 模式下有效，必须在 commit 前调用)
    ph_set_evt("CPU_CYCLES");
    ph_set_evt("INST_RETIRED");

    // 4. 提交配置
    ph_commit();

    // 5. 在关键路径进行采样 (热路径)
    for (int i = 0; i < 1000; i++) {
        ph_read(1, 1, 0.0);
        // ... 你的核心计算代码 ...
        ph_read(1, 2, 0.0);
    }

    // 6. 结束并刷盘
    ph_finalize();
    return 0;
}
```

### 3.2 MPI 程序示例

MPI 程序的使用方式类似，只需将前缀改为 `phmpi_`，并确保在 `MPI_Init` 之后、`MPI_Finalize` 之前调用：

```c
#include <mpi.h>
#include <ph_mpi.h>

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    phmpi_init("./ph_data");
    phmpi_set_tag(1, 1, "MPI_Work_Start");
    phmpi_set_tag(1, 2, "MPI_Work_End");
    phmpi_commit(); // 内部包含 MPI_Barrier 同步

    phmpi_read(1, 1, 0.0);
    // ... 计算与通信 ...
    phmpi_read(1, 2, 0.0);

    phmpi_finalize();
    MPI_Finalize();
    return 0;
}
```

## 4. 输出数据结构

运行结束后，数据会保存在 `ph_init` 指定的目录（如 `./ph_data`）下，每次运行自动递增生成 `run_<N>` 文件夹：

```text
ph_data/
├── run_info.csv              # 全局运行元信息
└── run_1/
    ├── ctag.csv              # 插桩点标签字典
    ├── etag.csv              # 性能事件字典
    ├── rankmap.csv           # Rank 到物理节点/CPU 的映射
    └── node01.example.com/   # 按主机名分类的目录
        ├── r0c0.csv          # Rank 0, CPU 0 的采样数据
        └── r1c1.csv          # Rank 1, CPU 1 的采样数据
```

详细的输出字段说明，请参考 [`docs/output_file_CN.md`](docs/output_file_CN.md)。内部使用的 GID 0 标记点（如 `PH_S_main`、`PH_S_ph_dump`）用于记录探针自身的开销与生命周期。

## 5. 案例与工具

你可以使用 `examples/` 目录下的代码复现案例。详见 [`examples/README.md`](examples/README.md)。

- `test1_asm_x86.c` / `test2_asm_armv8.c`：串行微内核插桩
- `test3_asmmpi_x86.c` / `test4_asmmpi_armv8.c`：MPI 并行插桩（如 `NP=16`）
- `test5_vardist_add.c`：性能波动分布
- `stream_mpi.c`：MPI STREAM，Copy/Scale/Add/Triad 区间计时

```bash
cd examples && make all
NP=16 ./run_mpi.sh stream_mpi.x
python3 summarize_run.py ./ph_data/stream_mpi
```

波动分析后处理可参考 `src/resolution_test/` 与 `src/varstat/`。

## 6. 当前局限性与开发计划

- 不支持在运行中动态更换进程与 CPU 核心的绑定关系。
- 暂不支持 OpenMP 线程级 profiling。
- Armv8-A 内核模块（`ph_enable_pmu.ko`）在部分环境下可能无法干净地卸载。
- 留有显式的 `ph_dump` 接口，API 调用次序要求严格，顺序错误可能导致运行异常。
- 后处理与统计分析工具（`ph_vis`, `ph_stat`）仍在完善中。
