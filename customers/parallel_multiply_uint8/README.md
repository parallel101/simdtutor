# 小彭老师 RGB24 格式图像处理性能优化案例

关于本问题的讨论：https://github.com/parallel101/simdtutor/issues/1

同学痛点：SIMD 只支持 4、8、16 大小的矢量处理，且多为 float 指令，缺少 u8 的乘法指令，对图像处理真不友好。

提出问题：如何实现 RGB24 格式的 uint8 图像的 mask 操作？

已知：
- mask：uint8 数组，大小 HxW
- img：uint8 数组，大小 HxWx3
- 想要进行的操作：`img = img * mask[..., None]`

测试设备：小彭老师的 Intel(R) Core(TM) i7-9750H CPU @ 2.60GHz，6 核 12 线程。

优化过程：
1. 原本 NumPy 进行 `img = img * mask[..., None]` 只有 238 it/s 非常慢。
2. 用 C++ 重写后（只使用 for 循环，不使用手动 SIMD 优化）得到 2068 it/s 的基础速度。
3. 小彭老师一开始落入教条思维，认为“要想SIMD化先转SOA”，提出可以改用 3xHxW 的 SOA 格式（相当于三张独立的 R、G、B 图片）然后让编译器自动矢量化，得到了 5460 it/s 的速度提升，但同学表示适应 SOA 需要大改他的源码。
4. 但后来想到可以保持 HxWx3 的 AOS 格式不变，但手动利用 _mm256_shuffle_epi8 对 mask 进行扩展，得到甚至比 SOA 还要快的 6828 it/s，且无需更改现有的 RGB24 数据排布。
5. 结合 OpenMP 并行化后达到了本案例的最终结果 15595 it/s，比 NumPy 快了 66 倍。

优化效果一览：

```
Numpy: 100%|█████████████████████████████████████████████████| 500/500 [00:02<00:00, 237.53it/s]
C++ for loop: 100%|█████████████████████████████████████████| 500/500 [00:00<00:00, 2067.54it/s]
C++ AVX2: 100%|███████████████████████████████████████████| 5000/5000 [00:00<00:00, 6828.48it/s]
C++ AVX2 full: 100%|██████████████████████████████████████| 5000/5000 [00:00<00:00, 6779.67it/s]
C++ for loop (OpenMP): 100%|████████████████████████████████| 500/500 [00:00<00:00, 4279.75it/s]
C++ AVX2 (OpenMP): 100%|█████████████████████████████████| 5000/5000 [00:00<00:00, 13370.96it/s]
C++ AVX2 full (OpenMP): 100%|████████████████████████████| 5000/5000 [00:00<00:00, 15594.82it/s]
```

这里可以看到如果只是给 for 循环加上 `#pragma omp parallel for` 后只能得到 4279 it/s，加速比感人，可见对于 RGB24 这种数据排布不平凡的情况，编译器难以自动优化，手动 SIMD 优化就非常必要。

# 最终优化完成品代码

```cpp
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <immintrin.h>
static void multiply_cpp_avx2_full(const char* img_HWN_ptr, const bool* mask_HW_ptr, 
                  char* output_HWN_ptr, size_t size) { // 此处的 output_HWN_ptr 也可以和 img_HWN_ptr 相等，实现就地 mask
    __m256i one = _mm256_set1_epi8(1);

    const size_t width = 256 / 8; // =32 uint8 per batch
    size_t batch = size / width;
        
    uint8_t * vectorIMG = reinterpret_cast<uint8_t*>(img_HWN_ptr);
    uint8_t * vectorMASK = reinterpret_cast<uint8_t*>(mask_HW_ptr);
    uint8_t * vectorResult = reinterpret_cast<uint8_t*>(output_HWN_ptr);
    uint8_t * vectorResult_end = vectorResult + batch * width * 3;
    __m256i shuf12 = _mm256_setr_epi8(0 ,0 ,0 ,1 ,1 ,1 ,2 ,2 ,2 ,3 ,3 ,3 ,4 ,4 ,4 ,5 ,
                                      5 ,5 ,6 ,6 ,6 ,7 ,7 ,7 ,8 ,8 ,8 ,9 ,9 ,9 ,10,10);
    __m256i shuf23 = _mm256_setr_epi8(5 ,5 ,6 ,6 ,6 ,7 ,7 ,7 ,8 ,8 ,8 ,9 ,9 ,9 ,10,10,
                                      10,11,11,11,12,12,12,13,13,13,14,14,14,15,15,15);
    __m256i shuf31 = _mm256_setr_epi8(10,11,11,11,12,12,12,13,13,13,14,14,14,15,15,15,
                                      0 ,0 ,0 ,1 ,1 ,1 ,2 ,2 ,2 ,3 ,3 ,3 ,4 ,4 ,4 ,5 );
    // REGISTER    01234567890123456789012345678901 01234567890123456789012345678901 01234567890123456789012345678901
    // img       = rgbrgbrgbrgbrgbrgbrgbrgbrgbrgbrg brgbrgbrgbrgbrgbrgbrgbrgbrgbrgbr gbrgbrgbrgbrgbrgbrgbrgbrgbrgbrgb
    // maskorig  = 0123456789ABCDEFGHIJKLMNOPQRSTUV
    // shufmask1 = 000111222333444555666777888999AA
    // shufmask2 = ABBBCCCDDDEEEFFFGGGHHHIIIJJJKKKL
    // shufmask3 = LLMMMNNNOOOPPPQQQRRRSSSTTTUUUVVV
    // 则 img_1 & shufmask1 就能起到和分别对 HWC 格式的前三分之一 rgb 进行 mask 一样的效果，依次类推
    while (vectorResult != vectorResult_end) {
        __m256i vec_mask = _mm256_loadu_si256((__m256i*) vectorMASK);
        vectorMASK += width;
        __m256i maskorig = _mm256_sub_epi8(vec_mask, one);
        __m256i shufmask2 = _mm256_shuffle_epi8(maskorig, shuf31);

        __m256i vec_img_1 = _mm256_loadu_si256((__m256i*) vectorIMG);
        __m256i maskorig_ll = _mm256_broadcastsi128_si256(_mm256_castsi256_si128(maskorig));
        vectorIMG += width;
        __m256i shufmask1 = _mm256_shuffle_epi8(maskorig_ll, shuf12);
        __m256i result_1 = _mm256_andnot_si256(shufmask1, vec_img_1);
        _mm256_storeu_si256((__m256i*) vectorResult, result_1);
        vectorResult += width;

        __m256i maskorig_hh = _mm256_broadcastsi128_si256(_mm256_extracti128_si256(maskorig, 1));
        __m256i vec_img_2 = _mm256_loadu_si256((__m256i*) vectorIMG);
        __m256i result_2 = _mm256_andnot_si256(shufmask2, vec_img_2);
        vectorIMG += width;
        _mm256_storeu_si256((__m256i*) vectorResult, result_2);

        __m256i shufmask3 = _mm256_shuffle_epi8(maskorig_hh, shuf23);
        vectorResult += width;
        __m256i vec_img_3  = _mm256_loadu_si256((__m256i*) vectorIMG);
        vectorIMG += width;
        __m256i result_3 = _mm256_andnot_si256(shufmask3, vec_img_3);
        _mm256_storeu_si256((__m256i*) vectorResult, result_3);
        vectorResult += width;
    }
    // 考虑边角料
    if (batch * width != size) [[unlikely]] {
        uint8_t * vectorResult_trueEnd = vectorResult + (size - batch * width) * 3;
        while (vectorResult != vectorResult_trueEnd) {
            uint8_t mask = *vectorMASK++;
            uint8_t r = *vectorIMG++;
            uint8_t g = *vectorIMG++;
            uint8_t b = *vectorIMG++;
            *vectorResult++ = r * mask;
            *vectorResult++ = g * mask;
            *vectorResult++ = b * mask;
        }
    }
}
static void multiply_cpp(char* img_HWN_ptr, bool* mask_HW_ptr, char* output_ptr, int N, int H, int W)
    if (N != 3) throw std::runtime_error("can only deal RGB24 for now");
    size_t size = H*W;
    const size_t chunk = 65536;
    #pragma omp parallel for
    for (size_t i = 0; i < size; i += chunk) {
        multiply_by(img_HWN_ptr + i*3, mask_HW_ptr + i, output_ptr + i*3, std::min(chunk, size - i));
    }
}
```

核心灵感：让 mask 从 (1, 0, 1, 0) 变成 (1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0)。

# 同学的原版仓库

本文件夹中所有代码原为同学为了展示其 RGB24 mask 操作实验代码所建立的仓库：https://github.com/chenxinfeng4/parallel_multiple_uint8

仓库通过 Cython 绑定，让 C++ 和 NumPy 的性能进行了对比。其中 fast_mask_lib.hpp 中是经过小彭老师优化的 C++ 代码，运行效果等价于 NumPy 的 `img = img * mask[..., None]`。

以下为原仓库中chenxinfeng4同学书写的README（教小彭老师如何安装和运行的）：

# 安装指南
---
```bash
git clone URL
cd GIT_PROJECT

python setup.py build_ext --inplace
```

## 速度测试
```bash
python speed_test.py
```


## 选项：是否使用多线程
打开`fast_mask_lib.hpp`. 修改配置后执行 `python setup.py build_ext --inplace` 。
```bash
void multiply_cpp(char* img_NHW_ptr, bool* mask_KNHW_ptr, char* output_ptr,
                   int K, int N, int H, int W)
{
    int size = N*H*W;
    // 开启 openmp 多线程加速
    // #pragma omp parallel for num_threads(3)
    for(int k=0; k<K; k++){
        char* output_NHW_ptr = &output_ptr[k*size];
        bool* mask_NHW_ptr = &mask_KNHW_ptr[k*size];
        MULTIPLY_BY(img_NHW_ptr, mask_NHW_ptr, 
                  output_NHW_ptr, size);
    }
}
```


## RGB24 的数据处理如何加速？
---
RGB24 的数据处理过程总， HWC 的格式处理特别慢。是否有合适的 SIMD 加速？
```python
# 3274.33it/s
for _ in tqdm.trange(5000):
    out2 = img_CHW * mask_HW

# 217.13it/s  非常慢
for _ in tqdm.trange(5000):
    out3 = img_HWC * mask_HW[..., None]
```
