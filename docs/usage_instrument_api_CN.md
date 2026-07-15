# PerfHound 插桩与性能分析使用指南（Arm 示例）

本文从工程工作过程出发，说明如何用 PerfHound（盘瓠）完成三类目标：

1. **插桩性能分析**：在代码关键路径打点，用周期/墙钟时间刻画区间耗时与热点。
2. **性能计数器采样**：在打点处同步读取硬件 PMU 事件（指令、缓存、分支、停顿等）。
3. **性能波动分析**：对同一区间做大量重复测量，统计分布、开销与可分辨的最小波动。

下文以 **Armv8-A** 平台、`PHASM` 直读 PMU、内存缓冲固定为 **4 KiB**（`PH_BUFSIZE=4`）为例。输出目录与 CSV 字段详见 [`output_file_CN.md`](output_file_CN.md)。

---

## 0. 三类目标与编译模式的对应关系

| 目标 | 典型 `PH_EVMODE` | 主要采集内容 | 后处理关注点 |
| ---- | ---------------- | ------------ | ------------ |
| 插桩性能分析 | `TS`（可加 `EV`） | `gid/pid`、CPU cycle、纳秒时间戳、可选 `uval` | 起止点差分 → 区间耗时；按标签定位热点 |
| 性能计数器采样 | `EV` 或 `EVX` | 上述字段 + 硬件事件计数 | `etag.csv` 对照 `ev1…evN`；区间内事件增量 |
| 性能波动分析 | 多为 `TS` | 同一区间大量重复采样 | 分布、分位数、开销、波动分辨率 |

编译期开关（`src/probe/make.config`）：

| 选项 | 取值 | 含义 |
| ---- | ---- | ---- |
| `PH_API` | `PHASM` / `PAPI` | Arm 上推荐 `PHASM`（直接访问 PMU 寄存器） |
| `PH_EVMODE` | `TS` / `EV` / `EVX` | 仅时间戳 / +最多 4 事件 / Arm 最多 12 事件 |
| `PH_PARMODE` | `SERIAL` / `MPI` | 串行或 MPI（Makefile 会同时产出两套库） |
| `PH_BUFSIZE` | 正整数（**单位 KiB**） | 本文固定为 `4`，即 4 KiB |
| `PH_DEBUG` | `YES` / `NO` | 调试日志；正式测量请用 `NO` |

**缓冲容量（4 KiB）**：`buf_nbyte = PH_BUFSIZE × 1024`，可容纳记录数约为 `4096 / sizeof(rec_t)`。

- `TS` / `EV`：`rec_t` 约 64 B → 约 **64** 条后自动刷盘。
- `EVX`（Arm）：`rec_t` 约 128 B → 约 **32** 条后自动刷盘。

缓冲满时 probe 自动调用 `ph_dump()` / `phmpi_dump()`；也可手动 `ph_dump()`，或在 `ph_finalize()` 时刷出剩余数据。

---

## 1. 工程工作过程总览

```text
① 平台准备（编译并加载 ph_enable_pmu.ko；`make check` 验证）
        ↓
② 配置并编译 PH-Probe（make.config → make && make install）
        ↓
③ 在被测程序中按固定顺序调用 API 插桩
        ↓
④ 编译链接被测程序（-lph 或 -lphmpi）并运行
        ↓
⑤ 读取 run_<N>/ 下 CSV，按目标做区间差分 / 事件分析 / 波动统计
```

三类目标共用同一套 API 调用链，差异主要在：

- 编译时选 `TS` 还是 `EV`/`EVX`；
- 是否调用 `ph_set_evt`；
- 插桩点如何成对布置、重复多少次；
- 后处理脚本如何解释 CSV。

---

## 2. 安装与编译（平台准备 + PH-Probe）

### 2.1 编译并加载 ph_enable_pmu.ko

x86 与 Arm 使用统一构建入口 `src/kmod/`，按本机架构生成 `ph_enable_pmu.ko`：

```bash
cd src/kmod
make
sudo insmod src/kmod/x86/ph_enable_pmu.ko       # x86-64
# 或
sudo insmod src/kmod/aarch64/ph_enable_pmu.ko   # Armv8-A
make check   # 第一步检查模块是否已加载；未加载则报错退出
```

- **Arm**：模块打开 `PMUSERENR_EL0` 等权限，使 `PHASM` 可在用户态读 PMU。
- **x86**：模块开启 `rdpmc`、暴露 sysfs 配置计数器，并尝试关闭 `nmi_watchdog` / 卸载 `iTCO_*`。

> 说明：部分环境卸载模块可能不干净；测量节点上保持加载即可。

### 2.2 配置并编译 PH-Probe

编辑 `src/probe/make.config`（Arm + 4 KiB 缓冲示例）：

```makefile
PREFIX     = .
PH_API     = PHASM
PH_EVMODE  = TS          # 计数器采样改为 EV 或 EVX
PH_PARMODE = SERIAL      # MPI 程序可写 MPI
PH_BUFSIZE = 4           # 4 KiB（不是 4096；4096 表示 4096 KiB）
PH_DEBUG   = NO
CC         = gcc
MPICC      = mpicc
```

按目标切换 `PH_EVMODE` 后重新编译：

```bash
cd src/probe
make clean && make && make install
```

安装产物（默认 `PREFIX=.`，即 `src/probe/` 下）：

| 产物 | 用途 |
| ---- | ---- |
| `lib/libph.so` | 串行插桩 |
| `lib/libphmpi.so` | MPI 插桩 |
| `include/perfhound.h` | 串行公共头文件 |

MPI 程序还需包含源码树中的 `include/ph_mpi.h`（`make install` 当前只复制 `perfhound.h`）。

修改 `make.config` 后必须重新 `make clean && make`，模式在编译期固化进动态库。

### 2.3 链接被测程序

串行：

```bash
gcc -O2 -fno-builtin \
  -I./src/probe/include -L./src/probe/lib \
  -o myapp.x myapp.c -lph
```

MPI：

```bash
mpicc -O2 -fno-builtin \
  -I./src/probe/include -L./src/probe/lib \
  -o myapp_mpi.x myapp.c -lphmpi
```

运行前保证动态库可被找到，例如：

```bash
export LD_LIBRARY_PATH=$PWD/src/probe/lib:$LD_LIBRARY_PATH
```

---

## 3. 关键 API 与强制调用顺序

应用侧**只应**包含公共头文件：

- 串行：`#include <perfhound.h>`
- MPI：`#include <ph_mpi.h>`

### 3.1 调用顺序（必须遵守）

```text
ph_init(path)                 # 或 phmpi_init；MPI 须在 MPI_Init 之后
  ├─ ph_set_tag(gid, pid, name)   # 可选，可多次；用户标签 gid ≥ 1
  ├─ ph_set_evt("EVENT_NAME")     # 仅 EV/EVX；须在 commit 之前；可多次
  ├─ ph_commit()                  # 提交配置，此后不可再 set_evt
  ├─ ph_read(gid, pid, uval)      # 热路径采样；可大量重复
  ├─ ph_dump()                    # 可选，强制刷缓冲
  └─ ph_finalize()                # MPI 对应 phmpi_*；须在 MPI_Finalize 之前
```

MPI 将前缀换成 `phmpi_`，语义对齐。`phmpi_set_tag` / `phmpi_commit` 内部含同步，所有 rank 都要走到这些调用。

### 3.2 各函数职责

| 函数 | 阶段 | 作用 |
| ---- | ---- | ---- |
| `ph_init(path)` | 初始化 | 创建数据目录树（默认 `./ph_data`）、分配 4 KiB 对齐缓冲、初始化计时器/PMU、注册 GID 0 内部标签 |
| `ph_set_tag(gid,pid,name)` | 注册 | 向 `ctag.csv` 写入插桩点可读名称 |
| `ph_set_evt(name)` | 注册 | 解析 Arm 事件名并写入 `etag.csv`；配置硬件事件码（commit 前） |
| `ph_commit()` | 提交 | 配置 PMU、写采样文件表头、置就绪；内部会打一条起始标记 |
| `ph_read(gid,pid,uval)` | 采样 | 写入一条 `rec_t`：标签、cycle、纳秒、`uval`、可选事件计数；缓冲将满时自动 dump |
| `ph_dump()` | 刷盘 | 将缓冲追加写入 `r<rank>c<cpu>.csv` |
| `ph_finalize()` | 收尾 | 结束标记、刷剩余数据、写 `run_info.csv`、释放资源 |

`uval` 为用户自定义 `double`（可传 `0`）。串行热路径请用 `ph_read`（`ph_fastread` 为 MPI 侧轻量接口，串行实现以 `ph_read` 为准）。

### 3.3 标签约定

| GID | PID | 含义 |
| --- | --- | ---- |
| 0 | 1 | Probe 开始（内部） |
| 0 | 2 | Probe 结束（内部） |
| 0 | 3 / 4 | dump 写盘开销标记（内部） |
| ≥ 1 | 用户自定义 | 业务插桩点 |

推荐成对打点：同一 `gid` 下用 `pid=1` 表示区间起点、`pid=2` 表示终点，后处理做差分即可得到区间耗时与事件增量。

---

## 4. 目标一：插桩性能分析（TS）

### 4.1 适用场景

定位函数/循环/通信段耗时，做代码级 profiling。只需时间戳时用 `PH_EVMODE=TS`，开销最小。

### 4.2 工作步骤

1. `make.config` 设 `PH_EVMODE=TS`，`PH_BUFSIZE=4`，编译安装 probe。
2. 在被测代码中：`init → set_tag → commit → read → finalize`（**不要**调用 `set_evt`）。
3. 在关心的代码段前后各调用一次 `ph_read`。
4. 运行后在 `run_<N>/<hostname>/r0c*.csv` 中按 `(gid,pid)` 配对做差分。

### 4.3 串行示例（Arm）

```c
#include <perfhound.h>

int main(void)
{
    ph_init("./ph_data");

    ph_set_tag(1, 0, "Kernel");
    ph_set_tag(1, 1, "Kernel_Start");
    ph_set_tag(1, 2, "Kernel_End");

    ph_commit();

    for (int i = 0; i < 1000; i++) {
        ph_read(1, 1, 0.0);   /* 起点 */
        /* ... 被测代码 ... */
        ph_read(1, 2, 0.0);   /* 终点 */
    }

    ph_finalize();
    return 0;
}
```

### 4.4 如何读结果

对相邻的 `(1,1)` 与 `(1,2)` 两行：

- `Δcycle = cycle_end − cycle_start`
- `Δns = nanosec_end − nanosec_start`

结合 `ctag.csv` 中的描述即可得到各插桩区间的耗时分布与热点排序。GID 0 的内部行在分析业务区间时应忽略。

---

## 5. 目标二：性能计数器采样（EV / EVX）

### 5.1 适用场景

在插桩点同步采集硬件事件，分析 CPI、缓存命中、分支误预测、前后端停顿等。

- `EV`：最多 **4** 个事件。
- `EVX`：Arm 最多 **12** 个事件。

### 5.2 工作步骤

1. `PH_EVMODE=EV` 或 `EVX`，`PH_API=PHASM`，`PH_BUFSIZE=4`，重新编译 probe。
2. 确认 `ph_enable_pmu.ko` 已加载（`lsmod | grep ph_enable_pmu` 或 `make -C src/kmod check`）。
3. `init → set_tag → set_evt（多次）→ commit → read → finalize`。
4. 用 `etag.csv` 的 `id` 顺序对应 CSV 中的 `ev1…evN` 列；区间分析同样对起止行做差分。

### 5.3 Arm 常用事件名（传给 `ph_set_evt`）

事件表见 `src/probe/include/ph_evt_aarch64.h`，名称大小写不敏感。常用示例：

| 类别 | 事件名示例 |
| ---- | ---------- |
| 周期 / 指令 | `CPU_CYCLES`, `INST_RETIRED`, `INST_SPEC` |
| 分支 | `BR_RETIRED`, `BR_MIS_PRED`, `BR_PRED` |
| L1 | `L1D_CACHE`, `L1D_CACHE_REFILL`, `L1I_CACHE`, `L1I_CACHE_REFILL` |
| L2 / L3 | `L2D_CACHE`, `L2D_CACHE_REFILL`, `L3D_CACHE` |
| TLB | `L1D_TLB`, `L1D_TLB_REFILL`, `DTLB_WALK` |
| 访存 / 停顿 | `MEM_ACCESS`, `STALL_FRONTEND`, `STALL_BACKEND` |

### 5.4 EV 模式示例

```c
#include <perfhound.h>

int main(void)
{
    ph_init("./ph_data");

    ph_set_tag(1, 1, "Region_Start");
    ph_set_tag(1, 2, "Region_End");

    ph_set_evt("CPU_CYCLES");
    ph_set_evt("INST_RETIRED");
    ph_set_evt("BR_RETIRED");
    ph_set_evt("L1D_CACHE");

    ph_commit();

    ph_read(1, 1, 0.0);
    /* ... 被测代码 ... */
    ph_read(1, 2, 0.0);

    ph_finalize();
    return 0;
}
```

EVX 可在 `commit` 前继续追加事件（最多 12 个），例如再增加 `L1I_CACHE`、`L2D_CACHE`、`L1D_CACHE_REFILL`、`L1D_TLB`、`MEM_ACCESS`、`STALL_FRONTEND`、`STALL_BACKEND` 等（参考 `examples/test2_asm_armv8.c` 的事件列表；该示例仍使用旧名 `pfh_*`，新代码请用 `ph_*`）。

### 5.5 MPI 计数器采样要点

```c
#include <mpi.h>
#include <ph_mpi.h>

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    phmpi_init("./ph_data");
    phmpi_set_tag(1, 1, "Work_Start");
    phmpi_set_tag(1, 2, "Work_End");
    phmpi_set_evt("CPU_CYCLES");
    phmpi_set_evt("INST_RETIRED");
    phmpi_commit();

    phmpi_read(1, 1, 0.0);
    /* ... */
    phmpi_read(1, 2, 0.0);

    phmpi_finalize();
    MPI_Finalize();
    return 0;
}
```

每个 rank 写入各自的 `run_<N>/<hostname>/r<rank>c<cpu>.csv`，无跨进程写同一文件。元数据（`ctag.csv`、`etag.csv`、`run_info.csv`）由 rank 0 维护。

---

## 6. 目标三：性能波动分析

### 6.1 适用场景

量化超算节点上同一代码区间的耗时抖动：测量开销、最小可测区间、波动分辨率，或收集大量样本做分布统计。

工程上通常仍用 **TS** 模式（降低插桩本身干扰），通过**大量重复**的成对 `ph_read` 采集样本。

### 6.2 工作步骤

1. 用 `PH_EVMODE=TS`、`PH_DEBUG=NO`、`PH_BUFSIZE=4` 编译 probe。
2. 固定进程/线程绑核（probe 不支持运行中改绑核）。
3. 对同一内核重复 N 次：`ph_read(gid,1)` → 内核 → `ph_read(gid,2)`。
4. 后处理：逐对差分得到区间样本，再算分位数、方差、分布直方图等。
5. 需要对照“理论周期 vs 实测”或搜索最小可分辨波动时，可参考 `src/resolution_test/`（当前实现以 x86 串行为主，流程可迁移到 Arm）。

### 6.3 推荐插桩模式

```c
ph_init("./ph_data");
ph_set_tag(1, 1, "Var_Start");
ph_set_tag(1, 2, "Var_End");
ph_commit();

for (int t = 0; t < NMEASURE; t++) {
    /* 可选：冲刷缓存，降低冷/热缓存混杂 */
    ph_read(1, 1, 0.0);
    /* 固定或可控长度的微内核 */
    ph_read(1, 2, 0.0);
}

ph_finalize();
```

`examples/test5_vardist_add.c` 展示了可变循环次数 + 缓存冲刷的波动分布采集思路（接口名为旧版 `pfh_*`，逻辑可直接映射到 `ph_*`）。

### 6.4 后处理思路

1. **区间化**：对每次测量，用终点行减去起点行，得到 `Δcycle` / `Δns`（及 EV 模式下的 `Δev*`）。
2. **开销估计**：空内核或极短内核的最小 `Δcycle` 可近似为插桩开销。
3. **波动刻画**：对大量 `Δcycle` 看分位数、IQR、尾部；比较不同内核长度下的分布分离程度，可估计“波动分辨率”。
4. **多级检测参考**：`src/resolution_test/x86/serial/main.sh` + `algo.py` 实现了 timing cost → least interval → variation resolution 三级流程；Arm 上可复用同一分析逻辑，仅替换被测微内核与 probe 链接方式。
5. **批量统计**：`src/varstat/` 提供多 run 汇总的 Python 辅助；若文件名布局与当前 `run_<N>/` 不一致，需按 [`output_file_CN.md`](output_file_CN.md) 适配路径。

---

## 7. 输出数据（简要）

`ph_init` / `phmpi_init` 指定的根目录下：

```text
<ph_root>/
├── run_info.csv
└── run_<N>/
    ├── ctag.csv          # 插桩点标签
    ├── etag.csv          # 事件名与编码（EV/EVX）
    ├── rankmap.csv       # rank → 主机/CPU
    └── <hostname>/
        └── r<rank>c<cpu>.csv   # 采样主数据
```

`r*.csv` 每行对应一次 `ph_read`：

```text
gid, pid, cycle, nanosec, uval [, ev1, ev2, ...]
```

完整列说明与多节点写入流程见 [`output_file_CN.md`](output_file_CN.md)。

---

## 8. 注意事项

1. **调用顺序不可打乱**：`set_evt` 必须在 `commit` 之前；`commit` 之后再 `set_evt` 会被忽略。
2. **GID 0 保留**：用户标签从 `gid ≥ 1` 开始。
3. **模式编译期固定**：换 `TS`/`EV`/`EVX` 或改 `PH_BUFSIZE` 后必须重编 `libph.so` / `libphmpi.so`。
4. **`PH_BUFSIZE` 单位是 KiB**：`4` = 4 KiB；仓库默认若为 `4096` 则表示 4 MiB，与本文示例不同。
5. **正式测量关闭调试**：`PH_DEBUG=NO`，并避免在 `ph_read` 热路径附近打印日志。
6. **需 kmod**：`PHASM` 依赖 `ph_enable_pmu.ko`；未加载时读计数器可能失败或得到无效值。
7. **绑核稳定**：不支持运行中改变进程与 CPU 的绑定；MPI 场景建议显式绑核以便 `r<rank>c<cpu>.csv` 含义清晰。
8. **旧示例命名**：`examples/` 与部分文档仍可能出现 `pfh_*` / `vt_*`；新代码统一使用 `ph_*` / `phmpi_*`。

---

## 9. 快速对照清单

| 步骤 | 插桩性能分析 | 计数器采样 | 波动分析 |
| ---- | ------------ | ---------- | -------- |
| kmod | 建议加载 | 必须加载 | 建议加载 |
| `PH_EVMODE` | `TS` | `EV` / `EVX` | `TS` |
| `PH_BUFSIZE` | `4` | `4` | `4` |
| `set_tag` | 是 | 是 | 是 |
| `set_evt` | 否 | 是 | 否 |
| `read` 模式 | 区间起止 | 区间起止 | 大量重复起止 |
| 后处理 | 差分耗时 | 差分 + 事件 | 分布 / 分辨率 |

按上述工作过程配置、插桩、运行并处理 CSV，即可在 Arm 上完成 PerfHound 的三类性能分析任务。
