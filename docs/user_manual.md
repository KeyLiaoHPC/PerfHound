# 盘瓠（PerfHound）

盘瓠（PerfHound）是一个代码级profiling工具套件，可用于并行代码性能分析和超算集群性能波动探测。

PerfHound is a toolset for code-level profiling. It is an ideal tool for performance analysis of performance applications, and performance variation detecting on HPC Clusters.

Both Chinses and English documentation can be found in **docs/**.

## 1 盘瓠的功能与特性

- 通过代码插桩的方式进行性能计数器的读取。
- 支持代码插桩位置的自定义标注。
- 支持rdtsc（x86）、多种计时器。

## 2 使用方法

### 2.1 系统和环境要求

### 2.2 编译与安装

#### 2.2.1 编译PerfHound API

#### 2.2.2 编译并加载内核模块

##### 1) 编译PerfHound API

```
// 在x86-64服务器上以默认配置编译
$ git clone https://git.computing.sjtu.edu.cn/keymorrislane/perfhound
$ cd perfhound/src
$ make
mpicc -O2 -fno-builtin -I/include -I/lustre/home/acct-hpc/hpckey/03-Project/perfhound/src/include -fPIC -DUSE_MPI  -DUSE_VARAPI  -DPFH_OPT_TS -I/lustre/home/acct-hpc/hpckey/03-Project/perfhound/src -lpfc -o varapi.mpi_ts.o -c api/varapi.c 
mpicc -O2 -fno-builtin -I/include -I/lustre/home/acct-hpc/hpckey/03-Project/perfhound/src/include -fPIC -DUSE_MPI  -DUSE_VARAPI  -DPFH_OPT_TS -I/lustre/home/acct-hpc/hpckey/03-Project/perfhound/src -lpfc -o file_op.mpi_ts.o -c api/file_op.c 
mpicc -O2 -fno-builtin -I/include -I/lustre/home/acct-hpc/hpckey/03-Project/perfhound/src/include -fPIC -DUSE_MPI  -DUSE_VARAPI  -DPFH_OPT_TS -I/lustre/home/acct-hpc/hpckey/03-Project/perfhound/src -lpfc -o vt_mpi.mpi_ts.o -c api/vt_mpi.c 
gcc -O2 -fno-builtin -I/include -I/lustre/home/acct-hpc/hpckey/03-Project/perfhound/src/include -fPIC  -lpfc  -lpfc -shared -o libvtapi_mpi_ts.so varapi.mpi_ts.o file_op.mpi_ts.o vt_mpi.mpi_ts.o
mpicc -O2 -fno-builtin -I/include -I/lustre/home/acct-hpc/hpckey/03-Project/perfhound/src/include -fPIC -DUSE_MPI  -DUSE_VARAPI  -DPFH_OPT_EV -I/lustre/home/acct-hpc/hpckey/03-Project/perfhound/src -lpfc -o varapi.mpi_ev.o -c api/varapi.c 
mpicc -O2 -fno-builtin -I/include -I/lustre/home/acct-hpc/hpckey/03-Project/perfhound/src/include -fPIC -DUSE_MPI  -DUSE_VARAPI  -DPFH_OPT_EV -I/lustre/home/acct-hpc/hpckey/03-Project/perfhound/src -lpfc -o file_op.mpi_ev.o -c api/file_op.c 
mpicc -O2 -fno-builtin -I/include -I/lustre/home/acct-hpc/hpckey/03-Project/perfhound/src/include -fPIC -DUSE_MPI  -DUSE_VARAPI  -DPFH_OPT_EV -I/lustre/home/acct-hpc/hpckey/03-Project/perfhound/src -lpfc -o vt_mpi.mpi_ev.o -c api/vt_mpi.c 
gcc -O2 -fno-builtin -I/include -I/lustre/home/acct-hpc/hpckey/03-Project/perfhound/src/include -fPIC  -lpfc  -lpfc -shared -o libvtapi_mpi_ev.so varapi.mpi_ev.o file_op.mpi_ev.o vt_mpi.mpi_ev.o
gcc -O2 -fno-builtin -I/include -I/lustre/home/acct-hpc/hpckey/03-Project/perfhound/src/include -fPIC  -DUSE_VARAPI  -DPFH_OPT_TS -I/lustre/home/acct-hpc/hpckey/03-Project/perfhound/src -lpfc -o varapi.ts.o -c api/varapi.c 
gcc -O2 -fno-builtin -I/include -I/lustre/home/acct-hpc/hpckey/03-Project/perfhound/src/include -fPIC  -DUSE_VARAPI  -DPFH_OPT_TS -I/lustre/home/acct-hpc/hpckey/03-Project/perfhound/src -lpfc -o file_op.ts.o -c api/file_op.c 
gcc -O2 -fno-builtin -I/include -I/lustre/home/acct-hpc/hpckey/03-Project/perfhound/src/include -fPIC  -lpfc  -lpfc -shared -o libvtapi_ts.so varapi.ts.o file_op.ts.o
gcc -O2 -fno-builtin -I/include -I/lustre/home/acct-hpc/hpckey/03-Project/perfhound/src/include -fPIC  -DUSE_VARAPI  -DPFH_OPT_EV -I/lustre/home/acct-hpc/hpckey/03-Project/perfhound/src -lpfc -o varapi.ev.o -c api/varapi.c 
gcc -O2 -fno-builtin -I/include -I/lustre/home/acct-hpc/hpckey/03-Project/perfhound/src/include -fPIC  -DUSE_VARAPI  -DPFH_OPT_EV -I/lustre/home/acct-hpc/hpckey/03-Project/perfhound/src -lpfc -o file_op.ev.o -c api/file_op.c 
gcc -O2 -fno-builtin -I/include -I/lustre/home/acct-hpc/hpckey/03-Project/perfhound/src/include -fPIC  -lpfc  -lpfc -shared -o libvtapi_ev.so varapi.ev.o file_op.ev.o
mkdir -p ./lib
mkdir -p ./include
mkdir -p ./bin
mv ./*.so ./lib
Pfh-Probe has been installed in .
```

完成后，会在src目录下生成./include/varapi.h和./lib/libvarapi\*.so。需要在环境变量CPATH、LIBRARY\_PATH和LD\_LIBRARY\_PATH中添加对应位置，在之后对被测代码进行插桩的时候，要将这些组件一起编译进被测代码。

##### 2) 在x86-64服务器上编译并加载libpfc内核模块

在x86\_64平台上，需要通过libpfc内核模块导出寄存器信息。首先编译libpfc（https://github.com/obilaniu/libpfc）。

编译完成后，加载libpfc内核模块。进入libpfc安装目录，以root权限运行modprobe ./libpfc.ko。若模块被正常加载，可以在/var/log/messages中看到pfc输出了CPU信息和部分寄存器信息。以下是在pi-2.0 CPU队列计算节点上的输出：

```
Mar 16 13:07:43 cas111 kernel: pfc: Kernel Module loading on processor Intel(R) Xeon(R) Gold 6248 CPU @ 2.50GHz (Family 6 (6), Model 85 (055), Stepping 7 (7))
Mar 16 13:07:43 cas111 kernel: pfc: cpuid.0x0.0x0:        EAX=00000016, EBX=756e6547, ECX=6c65746e, EDX=49656e69
Mar 16 13:07:43 cas111 kernel: pfc: cpuid.0x1.0x0:        EAX=00050657, EBX=02400800, ECX=7ffefbff, EDX=bfebfbff
Mar 16 13:07:43 cas111 kernel: pfc: cpuid.0x6.0x0:        EAX=00000ff7, EBX=00000002, ECX=00000009, EDX=00000000
Mar 16 13:07:43 cas111 kernel: pfc: cpuid.0xA.0x0:        EAX=07300804, EBX=00000000, ECX=00000000, EDX=00000603
Mar 16 13:07:43 cas111 kernel: pfc: cpuid.0x80000000.0x0: EAX=80000008, EBX=00000000, ECX=00000000, EDX=00000000
Mar 16 13:07:43 cas111 kernel: pfc: cpuid.0x80000001.0x0: EAX=00000000, EBX=00000000, ECX=00000121, EDX=2c100800
Mar 16 13:07:43 cas111 kernel: pfc: cpuid.0x80000002.0x0: EAX=65746e49, EBX=2952286c, ECX=6f655820, EDX=2952286e
Mar 16 13:07:43 cas111 kernel: pfc: cpuid.0x80000003.0x0: EAX=6c6f4720, EBX=32362064, ECX=43203834, EDX=40205550
Mar 16 13:07:43 cas111 kernel: pfc: cpuid.0x80000004.0x0: EAX=352e3220, EBX=7a484730, ECX=00000000, EDX=00000000
Mar 16 13:07:43 cas111 kernel: pfc: PM Arch Version:      4
Mar 16 13:07:43 cas111 kernel: pfc: Fixed-function  PMCs: 3#011Mask 0000ffffffffffff (48 bits)
Mar 16 13:07:43 cas111 kernel: pfc: General-purpose PMCs: 8#011Mask 0000ffffffffffff (48 bits)
Mar 16 13:07:43 cas111 kernel: pfc: Module pfc loaded successfully. Make sure to execute
Mar 16 13:07:43 cas111 kernel: pfc:     modprobe -ar iTCO_wdt iTCO_vendor_support
Mar 16 13:07:43 cas111 kernel: pfc:     echo 0 > /proc/sys/kernel/nmi_watchdog
Mar 16 13:07:43 cas111 kernel: pfc: and blacklist iTCO_vendor_support and iTCO_wdt, since the CR4.PCE register
Mar 16 13:07:43 cas111 kernel: pfc: initialization is periodically undone by an unknown agent.
```

然后需要关闭nmi\_watchdog，卸载iTCO\_wdt和iTCO\_vendor\_support两个模块（如果有）。

设置如下环境变量：

```
LIBPFC=~/apps/libpfc
PERFHOUND=~/apps/perfhound
LD\_LIBRARY\_PATH=$LIBPFC/lib:$PERFOUND/lib:$LD\_LIBRARY\_PATH
LIBRARY\_PATH=$LIBPFC/lib:$PERFOUND/lib:$LIBRARY\_PATH

```

然后可以运行pfcdemo检查libpfc是否已经正确加载。

##### 3) 在Armv8\_64平台上，编译并加载enable\_pmu内核模块。

#### 2.2.3 运行测试

##### 1) 修改被测代码。

以perhound/src/samples/stream\_mpi.c为例，对源代码进行插桩。

编译stream\_mpi.c。

```
$ cd samples && make

```

##### 2) 运行测试

按正常方式运行stream\_mpi。

运行后，进入数据文件夹./ph\_data，可以获取收集到的性能数据。

### 2.3 PerfHound API

#### 2.3.1 入门案例

此处以STREAM Benchmark为例，进行代码插桩。

#### 2.3.2 API列表

##### vt\_init 
初始化PerfHound API
##### vt\_set\_grp 
为插桩点编组。
##### vt\_set\_tag
为插桩点设置标记。
##### vt\_set\_evt
设置需要收集的性能事件。
##### vt\_commit 
提交PerfHound的配置。
##### vt\_strict\_sync
强制所有进程进行CPU计时器的对齐。
##### vt\_read
读取时间戳和性能计数器。
##### vt\_write
强制写入文件。
##### vt\_clean
写入并终止PerfHound。

### 2.4 性能数据分析

#### 2.4.1 PerfHound的输出数据

工程中包含如下文件：
- \<project\>: 以工程名称命名的根目录。
  - \<host\_dir\>：在某一节点上收集到的数据，以节点名称命名。
    - 采样数据文件：run\<RunID\>\_r\<RankID\>\_c\<CoreID\>\_all.csv
    - 采样日志文件：varapi\_run\<RunID\>\_\<HostName\>\_\<TimeStamp\>.log
  - run\<RunID\>\_ctags.csv
  - run\<RunID\>\_etags.csv
  - run\<RunID\>\_rankmap.csv
  - tstamp.log

#### 2.4.2 采样数据文件

采样数据文件的命名方式为run\<RunID\>\_r\<RankID\>\_c\<CoreID\>\_all.csv。其中，**RunID**表示数据来自应用的第几次运行，**RankID**表示被测量的MPI进程号，**CoreID**表示被测量的核心号。

每一条数据均由若干列组成，列之间通过逗号隔开，从左至右分别为ctag[0]、ctag[1]、cycle读数、纳秒读数、用户自定义的值、事件计数器1、事件计数器2……事件计数器的个数取决于运行pfh_ap时的设置。每个事件计数器的名称均记录在run\<RunID>\_etags.csv文件中，每个ctag的含义均记录在run\<RunID>_ctags.csv文件中。

一般而言，ctag[0]用于对有关联的采样点进行分组（例如某个函数当中的所有采样点，可以编为1组），ctag[1]则用于标记某个特定的采样点。通过两个ctag的组合，便可以定位任意一条数据在应用代码中的具体位置。

## 3 案例

你可以使用samples/目录下的代码，在自己的计算平台上复现如下案例，以便快速上手盘瓠。

### 3.2 HPL Benchmark 性能热点分析

### 3.2 STREAM Benchmark 性能波动数据收集

## 4 其他工具

## 5 开发计划

### 5.1 当前的局限性

- 不支持运行中更换进程与核心的绑定关系。
- 不支持OpenMP。
- armv8\_64的内核模块无法正常退出。
- 修改目标代码后，编译过程过于复杂，无法适配大型软件。
- 留有显式的write、commit、clean等接口，接口定义模糊，且需要遵循一定的调用次序，使用时容易发生错误。
- 未测试过对较复杂的科学计算软件进行性能收集。
- ph\_vis未完成。
- ph\_stat未完成。

### 5.2 下一版本需要解决的问题

