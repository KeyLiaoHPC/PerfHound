# 盘瓠（PerfHound）

盘瓠（PerfHound）是一个代码级 profiling 工具套件，可用于并行代码性能分析和超算集群性能波动探测。

详细 API 与 Arm 工作流见 [`usage_instrument_api_CN.md`](usage_instrument_api_CN.md)；输出格式见 [`output_file_CN.md`](output_file_CN.md)。

## 1 功能与特性

- 代码插桩读取时间戳与硬件 PMU 事件（x86-64 / Armv8-A）。
- 插桩点自定义标签（`ph_set_tag`）。
- MPI 场景按 `(hostname, rank, cpu)` 分文件写入，无锁争用。

## 2 使用方法

### 2.1 编译 PH-Probe

编辑 [`src/probe/make.config`](../src/probe/make.config)：

| 选项 | 含义 |
| ---- | ---- |
| `PH_API` | `PHASM`（直读 PMU）或 `PAPI` |
| `PH_EVMODE` | `TS` / `EV` / `EVX` |
| `PH_PARMODE` | `SERIAL` / `MPI`（Makefile 同时产出两套库） |
| `PH_BUFSIZE` | 缓冲大小，**单位 KiB** |
| `PH_DEBUG` | 正式测量请 `NO` |

```bash
cd src/probe
make clean && make && make install
```

产物：`src/probe/lib/libph.so`、`libphmpi.so`，头文件 `src/probe/include/perfhound.h`、`ph_mpi.h`。

### 2.2 内核模块（PHASM 推荐）

```bash
cd src/kmod
make
sudo insmod x86/ph_enable_pmu.ko          # 在 src/kmod 目录内
# 或 sudo insmod aarch64/ph_enable_pmu.ko
make check
```

卸载：`sudo rmmod ph_enable_pmu`

### 2.3 功能测试

```bash
make check    # 仓库根目录，见 tests/README.md
```

### 2.4 编译并运行 examples

```bash
cd examples
make all
export LD_LIBRARY_PATH=$PWD/../src/probe/lib:$LD_LIBRARY_PATH
NP=16 ./run_mpi.sh test3_asmmpi_x86.x
python3 summarize_run.py ./ph_data/test3_asmmpi_x86
```

每个样例的目的、期望结果与命令见 [`examples/README.md`](../examples/README.md)。

### 2.5 API 调用顺序

`ph_init` / `phmpi_init` → `ph_set_tag` → `ph_set_evt`（仅 EV/EVX，且在 `commit` 前）→ `ph_commit` → `ph_read` → `ph_finalize`。

串行头文件：`perfhound.h`（`ph_*`）。MPI：`ph_mpi.h`（`phmpi_*`）。**不要**包含内部头文件。

### 2.6 输出数据

```text
<ph_root>/
├── run_info.csv
└── run_<N>/
    ├── ctag.csv
    ├── etag.csv
    ├── rankmap.csv
    └── <hostname>/
        └── r<rank>c<cpu>.csv
```

列：`gid,pid,cycle,nanosec,uval[,ev1..evN]`。详见 [`output_file_CN.md`](output_file_CN.md)。

## 3 案例

- **串行微内核：** `examples/test1_asm_x86.c`
- **MPI 微内核：** `examples/test3_asmmpi_x86.c`
- **波动分布：** `examples/test5_vardist_add.c`
- **STREAM + MPI：** `examples/stream_mpi.c`

## 4 当前局限性

- 不支持运行中更换进程与 CPU 绑定。
- 不支持 OpenMP 线程级 profiling。
- API 调用顺序必须严格遵守；`ph_dump` 为可选刷盘接口。
