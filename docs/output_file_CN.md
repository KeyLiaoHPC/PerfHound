
## PerfHound 多节点多核输出数据组织形式

### 1. 目录树结构

每次运行生成一个递增编号的 `run_#` 子目录，整体层级如下：

```
<ph_root>/                          # 用户指定的数据根目录（默认 ./ph_data）
├── run_info.csv                    # 全局运行元信息（跨所有 run 追加）
├── tstamp.log                      # 时间戳日志
│
├── run_1/                          # 第 1 次运行
│   ├── ctag.csv                    # 插桩点标签定义（rank 0 写入）
│   ├── etag.csv                    # 性能事件定义（rank 0 写入）
│   ├── rankmap.csv                 # Rank → 主机/CPU 映射（每个 rank 各写一行）
│   │
│   ├── node01.example.com/         # 按主机名分目录
│   │   ├── r0c0.csv                # rank 0, CPU 0 的采样记录
│   │   ├── r0c1.csv                # rank 0, CPU 1 的采样记录
│   │   └── r1c0.csv                # rank 1, CPU 0 的采样记录
│   │
│   ├── node02.example.com/
│   │   ├── r2c0.csv
│   │   └── r3c0.csv
│   │
│   └── node03.example.com/
│       └── ...
│
└── run_2/                          # 第 2 次运行（目录自动递增）
    └── ...
```

**关键设计**：每个 `(rank, CPU)` 组合拥有**独立的 CSV 文件**（`r<rank>c<cpu>.csv`），存放在该 rank 所在主机名的子目录下。这样避免了多进程并发写同一文件的冲突。

---

### 2. 各 CSV 文件内容

#### 2.1 `rankmap.csv` — Rank 到物理位置的映射

| 列           | 含义                                          |
| ------------ | --------------------------------------------- |
| `rank`     | MPI rank ID                                   |
| `hostname` | 该 rank 运行在哪个节点                        |
| `cpu`      | 该 rank 绑定的 CPU 核心（`sched_getcpu()`） |
| `hosthead` | 预留字段（当前恒为 0）                        |
| `iorank`   | I/O 负责 rank（当前等于 rank 自身）           |

每个 rank 在初始化时各自往此文件追加一行。

#### 2.2 `ctag.csv` — 插桩点标签表

| 列              | 含义                                    |
| --------------- | --------------------------------------- |
| `gid`         | Group ID（GID 0 保留给 probe 内部标记） |
| `pid`         | Point ID                                |
| `description` | 人类可读的标签字符串                    |

由 `ph_set_tag()` / `phmpi_set_tag()` 写入（仅 rank 0）。系统预定义了以下标记：

| GID | PID | 含义                    |
| --- | --- | ----------------------- |
| 0   | 0   | PHGroup                 |
| 0   | 1   | PH Start / PH_S_main    |
| 0   | 2   | PH End / PH_E_main      |
| 0   | 3   | PH Wt Start（写入开始） |
| 0   | 4   | PH Wt End（写入结束）   |

#### 2.3 `etag.csv` — 性能事件定义表

| 列       | 含义                                       |
| -------- | ------------------------------------------ |
| `id`   | 事件序号                                   |
| `name` | 事件名称字符串（如`"INST_RETIRED.ANY"`） |
| `code` | 硬件事件编码（16 进制）                    |

由 `ph_set_evt()` / `phmpi_set_evt()` 写入（仅 rank 0）。

#### 2.4 `r<rank>c<cpu>.csv` — 核心采样数据

每行对应一次 `ph_read()` 调用：

| 列                 | 类型         | 含义                                               |
| ------------------ | ------------ | -------------------------------------------------- |
| `gid`            | uint32       | Group ID                                           |
| `pid`            | uint32       | Point ID                                           |
| `cycle`          | int64/uint64 | CPU 周期计数                                       |
| `nanosec`        | int64/uint64 | 纳秒级绝对时间戳                                   |
| `uval`           | double       | 用户自定义浮点值                                   |
| `ev1` … `evN` | uint64/int64 | 硬件性能事件计数器（1~4 个，EVX 模式最多 8/12 个） |

列数由编译时的 `PH_EVMODE` 决定：

- **TS 模式**：只有时间戳，无 `ev` 列
- **EV 模式**：最多 4 个事件列
- **EVX 模式**：x86 最多 8 列，Arm 最多 12 列

#### 2.5 `run_info.csv` — 全局运行元信息

| 列              | 含义                            |
| --------------- | ------------------------------- |
| `id`          | 运行编号（与`run_#` 对应）    |
| `nrank`       | 总 rank 数                      |
| `nevent`      | 配置的事件数                    |
| `ngroup`      | 总记录数                        |
| `start_time`  | 开始时间（`YYYYMMDDTHHmmss`） |
| `end_time`    | 结束时间                        |
| `description` | 描述（当前为空）                |

由 rank 0 在 `ph_finalize()` 时追加一行。

---

### 3. 写入流程（多节点多核场景）

```
phmpi_init(path)
├── rank 0: 创建 <ph_root>/ 和 run_#/, 写 ctag.csv/etag.csv/rankmap.csv 表头
├── MPI_Barrier
├── 所有 rank: 计算本地路径 run_#/<hostname>/r<rank>c<cpu>.csv
├── 环形链式初始化（rank 0→1→2→...→N-1）：
│   └── 每个 rank: 创建 <hostname>/ 目录 + 写 rankmap.csv 一行 + 创建 r<rank>c<cpu>.csv
└── MPI_Barrier

phmpi_read() / phmpi_fastread()
├── 写入内存缓冲 rec_t* ph_precs（默认 4 KiB）
└── 缓冲满时自动调用 phmpi_dump()

phmpi_dump() / phmpi_finalize()
├── 每个 rank 独立追加写入自己的 r<rank>c<cpu>.csv
└── rank 0 追加 run_info.csv
```

**核心要点**：

- **按 `(hostname, rank, cpu)` 三级维度拆分文件**，每个 rank 只写自己的文件，无需文件锁
- **元数据文件**（ctag、etag、run_info）由 rank 0 独占写入
- **rankmap** 由每个 rank 各自追加一行
- 缓冲机制（默认 4 KiB ≈ 128 条记录）减少 I/O 频次，缓冲满或 finalize 时刷盘
