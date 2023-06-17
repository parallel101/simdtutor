# x86-64 SIMD 矢量优化系列教程（绝赞连载中）

第一课视频：https://www.bilibili.com/video/BV1cu411a71b

## 目录结构

- result 文件夹中为罗列各种不同的优化方案，及其在小彭老师的电脑上的所花时间等信息。

- source 文件夹中包含不同任务的最终优化方案、原始标量版实现、测试用例、和性能测试。

- main.cpp 为实验文件。需测试其他任务时，要把该任务源文件替换 main.cpp：

```bash
cp source/findp.cpp main.cpp  # 测试 findp 任务
```

- CMakeLists.txt 用于编译 main.cpp 文件并链接上 benchmark 和 gtest 库。

- watcher.sh 是服务于 Linux 用户的自动化测试运行工具。

- animations 一系列演示 SIMD 原理的自制 manim 动画，[B 站视频链接](https://www.bilibili.com/video/BV1nX4y147aB)。

## 实验需求

硬件要求：支持 AVX2 的 x86 CPU，Intel 和 AMD 均可，8 GB 内存。
硬件最低要求：64 位的 x86 CPU，Intel 和 AMD 均可，2 GB 内存。

> 所有 64 位 CPU 均支持 SSE，过老的硬件可能无法运行部分含 AVX 的实验代码。

Linux 做实验所需包（以 Arch Linux 为例）：

```bash
pacman -S inotify-tools gtest benchmark cmake gcc python sed cpu-x
```

Wendous 做实验所需安装项：

- Virtual Stdio 2019 或以上
- CMake 3.18 或以上（VS 通常会自带）
- Google Test 预安装或自行 add_subdirectory 改造
- Google Benchmark 预安装或自行 add_subdirectory 改造
- CPU-Z 硬件检测器（如果你对研究感兴趣的话）

> 推荐使用优化能力较强的 GCC 和软件包版本始终保持最新的 Arch Linux 系统，不要使用影响性能的 WSL 和 Docker。

优先级：本地 Linux > 租借服务器 > MaikeOS > Wendous > WSL

## 构建项目

建议用 CMake 构建：

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
build/main
```

如需手动构建，GCC 推荐编译参数：

```bash
g++ -lbenchmark -lgtest -lbenchmark_main -std=c++20 -mavx2 -mfma -O3 main.cpp -o a.out && ./a.out
```

如需手动构建，MSVC 推荐编译参数：

```cmd
/std:c++20 /arch:AVX2 /O2
```

## Linux 建议用 watcher.sh - 自动化的实验小助手

如果是 Linux 系统，可以在另一个终端运行 watcher.sh，每当你写入时他就会自动编译（他通过 inotify 监视你的写入动作），并把实验结果输出到 /tmp/watched.s 和 /tmp/benched.cpp，你可以用任何编辑器打开这两个文件，他们是实时更新的，watcher.sh 中的 cflags 变量可以修改编译参数。

如果你使用 vim，可以导入了本目录的 .vim_localrc，则打开 main.cpp 时也会自动打开反汇编和性能测试结果窗口，每次保存时窗口会实时更新，性能测试结果窗口（/tmp/benched.cpp）会记录你的每一次修改草案，方便你进行性能对比，反汇编窗口中是关键函数（包在 BEGIN CODE 和 END CODE之间）所生成的汇编。

性能测试结果窗口中（和 result 文件夹中相同）会包含一行，#if _ 是为了防止 clangd 报错请无视。其中 ns 数据表示纳秒，不同的电脑上可能随 CPU 性能的不同而不同。其中的 CPI 指标是 Clock-Per-Item，处理每个所需的时间，该数值是把纳秒数据乘以你的 CPU 主频得出的，在不同电脑上有一定可比性，且可以结合 Intel Intrinsics Guide 的 CPI（Clock-Per-Instruction）计算得出的理论值进行对比，加深理解。

## 参考资料

Intel Intrinsics Guide 在线参考资料: https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html

Intel 64 and IA-32 Architectures Optimization Reference Manual 手册: http://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-optimization-manual.pdf

Vectorclass 第三方纯头文件库，2.0 版本: https://github.com/vectorclass/version2

CPU-Z 指令集扩展检测工具：https://www.cpuid.com/softwares/cpu-z.html

## 相关头文件

| Header        | Extension(s)                        |
|---------------|-------------------------------------|
| <mmintrin.h>  | MMX                                 |
| <xmmintrin.h> | SSE                                 |
| <emmintrin.h> | SSE2                                |
| <pmmintrin.h> | SSE3                                |
| <tmmintrin.h> | SSSE3                               |
| <smmintrin.h> | SSE4.1                              |
| <nmmintrin.h> | SSE4.2                              |
| <wmmintrin.h> | AES                                 |
| <immintrin.h> | AVX, AVX2, FMA, BMI, POPCNT, AVX512 |
| <x86intrin.h> | Auto (GCC)                          |
| <intrin.h>    | Auto (MSVC)                         |

建议导入 <immintrin.h>，自动包含所有指令集扩展。

注：本课程主打一个 SSE，介绍的指令集范围从 MMX 到 AVX2，均基于 x86 架构，暂时没有讲解 ARM NEON 指令集的计划，若为 ARM 硬件则可尝试用 sse2neon.h 兼容层实验，由于教师电脑不支持 AVX-512 所以也没有 AVX-512 的计划 <del>但可以长按一键三连赞助小彭老师购买</del>。
