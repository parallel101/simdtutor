# x86-64 SIMD矢量优化系列教程（施工中）

可以先预习以下参考资料：

## Intel Intrinsics Guide

https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html

## Intel 64 and IA-32 Architectures Optimization Reference Manual

http://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-optimization-manual.pdf

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
