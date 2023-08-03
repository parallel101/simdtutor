# x86-64 SIMD 矢量优化系列教程（绝赞连载中）

第一课视频：https://www.bilibili.com/video/BV1cu411a71b

## 性能尴尬？不会优化？小彭老师推出义务优化服务

你有关键代码需要优化，但是不知道怎么做？例如你的项目中有一段非常慢的瓶颈代码，可以在本仓库里开一个 Issue 让专业的小彭老师帮你优化，并在对话中学习优化知识。同时，所有的对话和代码对后来者可见，方便具有相似问题的同学直接找到答案。

我们的口号是：**优化知识，永远不应该束之高阁，永远不应该无法检索，永远不应该成为少数人的专利**。

### 常见痛点

你试图用小彭老师公开课上教的知识自己用 SIMD 或者 OpenMP 优化，但是发现效果不明显，可能是使用的姿势不对。小彭老师课程只介绍了最基本的情况，并不能覆盖所有领域的多元化需求。

你看了一两节课，或是自己看 Intel 官方的指令参考手册，或是调用 xsimd 等语法糖包装库能够做出的优化，都是你的代码较为简单的情况，这时**编译器大多也能自动做出**，用不着手动优化。

而较为复杂的情况，你自己也优化不来，**编译器也优化不来**。为了最大化性能，需要结合**具体情况具体分析**，手动优化。这时你需要的是小彭老师这样的优化专家，结合他多年的优化经验，不失时机地进行一些灵感迸发，才能优化到逼近 CPU 的理论 Flops 上限。

不仅 SIMD 优化是如此，并行优化往往也会遇到这种情况：**你加上了 `#pragma omp parallel for` 但收效甚微**。

小彭老师意识到：对于性能优化这一特定领域，仅仅提供通用的课程是远远不够的。为了能够了解各位同学在工作中遇到的实际情况，小彭老师决定推出义务优化服务，你可以贴出自己的代码让专业的小彭老师帮你分析。毕竟问题是无限的，一节优化课不可能覆盖所有的问题，不如让教学跟着具体的问题走。

### 优秀提问范例

提问的方式有没有什么参考模板？可以参考[这位同学](https://github.com/parallel101/simdtutor/issues/1)，他用 numpy 做图像的蒙版操作，觉得非常慢。于是他通过 Cython 调用 C++ 手写了蒙版操作的代码，发现居然比 NumPy 还慢了，非常困惑。最后他试图用 SIMD 指令优化，但他的图像是 RGB 格式（而不是 RGBA），不知道怎么把 3 通道的颜色数据适配到大小固定为 32 字节的 AVX 寄存器里去。于是请教小彭老师帮他优化，最后比原版 NumPy 快 **66 倍**。

### 提供关键部位的源码

光用语言是很难描述问题的，**请给出有待优化的代码**，这样小彭老师才能分析为什么慢。

如果你的项目还比较小，可以把项目上传到 GitHub 上，并创建 Issue 附上项目仓库链接，然后指出哪个文件的哪一段代码是你急需优化的。

如果是比较庞大的项目，直接给小彭老师看全部代码会让他找不到要优化的点。可以单独创建一个仓库，放入要优化的瓶颈代码。

具有很多依赖项的项目，会让小彭老师感到不熟悉，使小彭老师难以上手，所以十分建议先抽出关键代码，单独创建一个仓库方便小彭老师复现，同时也方便归档供后来的同学查阅。

参考 #1 同学的做法，他是一个基于 opencv-python 的项目，除了要优化的 mask 操作还有很多其他的操作，但是他**专门创建了一个仓库**，**里面只有要优化的代码和相关测试框架**，没有多余的依赖项，让小彭老师能够直接上手复现结果。

### 性能必须是可复现的

要求可以测出时间，一般来说会用 chrono 或者 clock 等函数测时间，如果用了 Google Benchmark 等专业的测试框架是最好的，例如 #1 同学采用了 Python 中的 tqdm 框架，这些性能测试框架都会多次循环以求得更准确的时间值，避免偶然误差和预热缓存所需的时间。

总之，一定要**确保小彭老师能够复现你的结果**，以测时间的形式直观衡量代码的快慢。并且案例尽可能最小化，让小彭老师容易把握关键问题。

如果使用了外部文件数据（例如 obj 网格模型等），请在 GitHub 仓库中提供数据，如果数据文件太大不适合上传 GitHub，也可以上传到百度网盘并在 Issue 正文中贴出链接。

### 优化服务仅限 CPU

小彭老师的知识范围主要限于 CPU 高性能并行优化（TBB、OpenMP 等），对于 CUDA 的了解相对较少（仅做了两节 CUDA 专题课），自用的显卡型号也较为落后（RTX2080）。并且 CUDA 项目安装和复现困难（经常卡编译器版本和显卡型号），因此**原则上不会接受 CUDA 方面的优化提问**，特别是和 AI 有关的动不动要配一堆 anaconda 虚拟环境的所谓“算子优化”提问（AI 什么的最讨厌了），但是 OpenGL 的 shader 优化等问题都可以提。此外，由于小彭老师不具有多台超算资源，对于多节点的并行（例如 MPI）也无能为力复现。

> 小彭老师并不是讨厌 GPU 本身，如果是用 CUDA 做物理仿真，甚至是图像处理，也都是欢迎提问的，只不过现在 GPU 被滥用做 G 以外的业务，挤压我们“臭打游戏”的发展空间，所以会特别反感 CUDA。

对于实在没法抽出关键代码单独运行的项目，也可提供向日葵（sunlogin）等远程控制软件的途径，让小彭老师直接访问你配置好的环境里的项目，但这样会让其他同学无法看到问题解决的过程（既然享有小彭老师免费性能优化的权利，也要提供方便其他后来同学检索的义务）。

### 检索历史归档

所有优化成功的案例都会放入 customers 文件夹留档，供后来的同学学习参考。小彭老师和同学的所有讨论全部在 Issue 页面公开，以供检索，任何有相似情形的后来者都可以直接在 Issue 中找到答案。如果现有的 Issue 都没有和你类似的情况，请积极提问，填补空白，这样后来的同学也能受益（既享有小彭老师免费性能优化的权利，也享有提出“好问题”方便后来同学检索的义务）。

### 拒绝对非瓶颈代码的盲目优化

例如你有一个创建窗体的函数 createWindow(string title)，你说这里要不要改成 createWindow(string const &title)，更高效？可以是可以，但是这种函数往往不是瓶颈所在，除非你的程序每秒创建 1000000 个窗口，并且除了创建窗口外什么计算任务也不做。

更好的例子是，你这个窗体显示的是物理仿真的结果，而物理仿真是真正耗时的任务，因此你应该考虑对物理仿真的算法进行优化（并行、SIMD 化等），用 CUDA 的话说，就是要抓住 kernel（笑）。

至理名言：**如果马桶堵了，那么你给自己的屁屁再怎么涂甘油润滑，都是没用的**。屁屁优化得再快，照样卡在马桶这儿下不去，这时急需优化的是马桶而不是屁屁。如果法线首先应该通过 profiler 工具，或者手动 chrono 检测每一段代码的运行时间，确定最耗时的那一部分（称之为热代码），进行优化。而不是对压根不是瓶颈的冷代码优化半天，反而优化出 BUG 来。

总之，如果你连代码运行总时间都没测试过，还是不要心理作用优化了，用 C++11 的 chrono 测一下所费时间是最起码的：

```cpp
#include <chrono>

auto t0 = std::chrono::high_resoltuion_clock::now();
此处是你待测的代码
auto t1 = std::chrono::high_resoltuion_clock::now();
std::cout << "我的代码花了" << std::chrono::duration_cast<std::chrono::duration<double>>(t1 - t0).count() << "秒" << std::endl;
```

你连这个时间都不知道，怎么知道优化前后是提升了还是倒退了？全靠“想当然”优化？

如果是 C 语言，也可以用 `clock()` 函数测。当然有条件的最好还是用一下 Google Benchmark 等基准测试框架。

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

- customers 一些成功的客户优化案例。

## 实验需求

- 硬件要求：支持 AVX2 的 x86 CPU，Intel 和 AMD 均可，8 GB 内存。
- 硬件最低要求：64 位的 x86 CPU，Intel 和 AMD 均可，2 GB 内存。

> 注：所有 64 位 CPU 均支持 SSE，过老的硬件可能无法运行部分含 AVX 的实验代码。

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
