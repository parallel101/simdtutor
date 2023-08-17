编译期静态分发，根据是否指定了 -mavx2 参数：

```cpp
#ifdef __AVX2__
#include <immintrin.h>
#endif

void rgba2rgb(uint8_t const *in_rgba, uint8_t *out_rgb, size_t n) {
#ifdef __AVX2__
    const __m128i shuf1 = _mm_setr_epi8(0,1,2,4,5,6,8,9,10,12,13,14,3,7,11,15);
    const __m128i shuf2 = _mm_setr_epi8(5,6,8,9,10,12,13,14,3,7,11,15,0,1,2,4);
    const __m128i shuf3 = _mm_setr_epi8(10,12,13,14,3,7,11,15,0,1,2,4,5,6,8,9);
    const __m128i shuf4 = _mm_setr_epi8(3,7,11,15,0,1,2,4,5,6,8,9,10,12,13,14);
    auto in_rgba_end = in_rgba + ((n - 16) / 16 * 16) * 4;
    auto in_rgba_true_end = in_rgba + n * 4;
    while (in_rgba < in_rgba_end) {
        __m128i v1_rgba = _mm_loadu_si128((__m128i *)in_rgba); in_rgba += 16;
        __m128i v2_rgba = _mm_loadu_si128((__m128i *)in_rgba); in_rgba += 16;
        __m128i v3_rgba = _mm_loadu_si128((__m128i *)in_rgba); in_rgba += 16;
        __m128i v4_rgba = _mm_loadu_si128((__m128i *)in_rgba); in_rgba += 16;
        __m128i v1_rgb = _mm_shuffle_epi8(v1_rgba, shuf1);
        __m128i v2_rgb = _mm_shuffle_epi8(v2_rgba, shuf2);
        __m128i v3_rgb = _mm_shuffle_epi8(v3_rgba, shuf3);
        __m128i v4_rgb = _mm_shuffle_epi8(v4_rgba, shuf4);
        __m128i v1e_rgb = _mm_blend_epi32(v1_rgb, v2_rgb, 0b1000);
        __m128i v2e_rgb = _mm_blend_epi32(v2_rgb, v3_rgb, 0b1100);
        __m128i v3e_rgb = _mm_blend_epi32(v3_rgb, v4_rgb, 0b1110);
        _mm_storeu_si128((__m128i *)out_rgb, v1e_rgb); out_rgb += 16;
        _mm_storeu_si128((__m128i *)out_rgb, v2e_rgb); out_rgb += 16;
        _mm_storeu_si128((__m128i *)out_rgb, v3e_rgb); out_rgb += 16;
    }
    while (in_rgba != in_rgba_true_end) {
        *out_rgb++ = *in_rgba++;
        *out_rgb++ = *in_rgba++;
        *out_rgb++ = *in_rgba++;
        in_rgba++;
    }
#else
    for (size_t i = 0; i < n; i++) {
        out_rgb[i * 3 + 0] = in_rgba[i * 4 + 0];
        out_rgb[i * 3 + 1] = in_rgba[i * 4 + 1];
        out_rgb[i * 3 + 2] = in_rgba[i * 4 + 2];
    }
#endif
}
```

运行时动态分发，根据运行时检测到的 cpuid 自动决定调用哪个版本：

```cpp
#ifdef __x86_64__
#include <immintrin.h>
#endif

__attribute__((__target__("avx2"))) void rgba2rgb(uint8_t const *in_rgba, uint8_t *out_rgb, size_t n) {
    const __m128i shuf1 = _mm_setr_epi8(0,1,2,4,5,6,8,9,10,12,13,14,3,7,11,15);
    const __m128i shuf2 = _mm_setr_epi8(5,6,8,9,10,12,13,14,3,7,11,15,0,1,2,4);
    const __m128i shuf3 = _mm_setr_epi8(10,12,13,14,3,7,11,15,0,1,2,4,5,6,8,9);
    const __m128i shuf4 = _mm_setr_epi8(3,7,11,15,0,1,2,4,5,6,8,9,10,12,13,14);
    auto in_rgba_end = in_rgba + ((n - 16) / 16 * 16) * 4;
    auto in_rgba_true_end = in_rgba + n * 4;
    while (in_rgba < in_rgba_end) {
        __m128i v1_rgba = _mm_loadu_si128((__m128i *)in_rgba); in_rgba += 16;
        __m128i v2_rgba = _mm_loadu_si128((__m128i *)in_rgba); in_rgba += 16;
        __m128i v3_rgba = _mm_loadu_si128((__m128i *)in_rgba); in_rgba += 16;
        __m128i v4_rgba = _mm_loadu_si128((__m128i *)in_rgba); in_rgba += 16;
        __m128i v1_rgb = _mm_shuffle_epi8(v1_rgba, shuf1);
        __m128i v2_rgb = _mm_shuffle_epi8(v2_rgba, shuf2);
        __m128i v3_rgb = _mm_shuffle_epi8(v3_rgba, shuf3);
        __m128i v4_rgb = _mm_shuffle_epi8(v4_rgba, shuf4);
        __m128i v1e_rgb = _mm_blend_epi32(v1_rgb, v2_rgb, 0b1000);
        __m128i v2e_rgb = _mm_blend_epi32(v2_rgb, v3_rgb, 0b1100);
        __m128i v3e_rgb = _mm_blend_epi32(v3_rgb, v4_rgb, 0b1110);
        _mm_storeu_si128((__m128i *)out_rgb, v1e_rgb); out_rgb += 16;
        _mm_storeu_si128((__m128i *)out_rgb, v2e_rgb); out_rgb += 16;
        _mm_storeu_si128((__m128i *)out_rgb, v3e_rgb); out_rgb += 16;
    }
    while (in_rgba != in_rgba_true_end) {
        *out_rgb++ = *in_rgba++;
        *out_rgb++ = *in_rgba++;
        *out_rgb++ = *in_rgba++;
        in_rgba++;
    }
}

__attribute__((__target__("sse4.1"))) void rgba2rgb(uint8_t const *in_rgba, uint8_t *out_rgb, size_t n) {
    const __m128i shuf1 = _mm_setr_epi8(0,1,2,4,5,6,8,9,10,12,13,14,3,7,11,15);
    const __m128i shuf2 = _mm_setr_epi8(5,6,8,9,10,12,13,14,3,7,11,15,0,1,2,4);
    const __m128i shuf3 = _mm_setr_epi8(10,12,13,14,3,7,11,15,0,1,2,4,5,6,8,9);
    const __m128i shuf4 = _mm_setr_epi8(3,7,11,15,0,1,2,4,5,6,8,9,10,12,13,14);
    auto in_rgba_end = in_rgba + ((n - 16) / 16 * 16) * 4;
    auto in_rgba_true_end = in_rgba + n * 4;
    while (in_rgba < in_rgba_end) {
        __m128i v1_rgba = _mm_loadu_si128((__m128i *)in_rgba); in_rgba += 16;
        __m128i v2_rgba = _mm_loadu_si128((__m128i *)in_rgba); in_rgba += 16;
        __m128i v3_rgba = _mm_loadu_si128((__m128i *)in_rgba); in_rgba += 16;
        __m128i v4_rgba = _mm_loadu_si128((__m128i *)in_rgba); in_rgba += 16;
        __m128i v1_rgb = _mm_shuffle_epi8(v1_rgba, shuf1);
        __m128i v2_rgb = _mm_shuffle_epi8(v2_rgba, shuf2);
        __m128i v3_rgb = _mm_shuffle_epi8(v3_rgba, shuf3);
        __m128i v4_rgb = _mm_shuffle_epi8(v4_rgba, shuf4);
        __m128i v1e_rgb = _mm_blend_epi16(v1_rgb, v2_rgb, 0b11000000);
        __m128i v2e_rgb = _mm_blend_epi16(v2_rgb, v3_rgb, 0b11110000);
        __m128i v3e_rgb = _mm_blend_epi16(v3_rgb, v4_rgb, 0b11111100);
        _mm_storeu_si128((__m128i *)out_rgb, v1e_rgb); out_rgb += 16;
        _mm_storeu_si128((__m128i *)out_rgb, v2e_rgb); out_rgb += 16;
        _mm_storeu_si128((__m128i *)out_rgb, v3e_rgb); out_rgb += 16;
    }
    while (in_rgba != in_rgba_true_end) {
        *out_rgb++ = *in_rgba++;
        *out_rgb++ = *in_rgba++;
        *out_rgb++ = *in_rgba++;
        in_rgba++;
    }
}

__attribute__((__target__("default"))) void rgba2rgb(uint8_t const *in_rgba, uint8_t *out_rgb, size_t n) {
    for (size_t i = 0; i < n; i++) {
        out_rgb[i * 3 + 0] = in_rgba[i * 4 + 0];
        out_rgb[i * 3 + 1] = in_rgba[i * 4 + 1];
        out_rgb[i * 3 + 2] = in_rgba[i * 4 + 2];
    }
}
```

运行时动态分发，但都让编译器自动根据检测到的 CPU 架构针对性地优化：

```cpp
__attribute__((target_clones("sse4.1,avx"))) void rgba2rgb(uint8_t const *in_rgba, uint8_t *out_rgb, size_t n) {
    for (size_t i = 0; i < n; i++) {
        out_rgb[i * 3 + 0] = in_rgba[i * 4 + 0];
        out_rgb[i * 3 + 1] = in_rgba[i * 4 + 1];
        out_rgb[i * 3 + 2] = in_rgba[i * 4 + 2];
    }
}
```

用户自定义的运行时分发规则，手动使用 `__builtin_cpu_supports` 检测：

```cpp
__attribute__((ifunc("rgba2rgb_dispatch"))) void rgba2rgb(uint8_t const *in_rgba, uint8_t *out_rgb, size_t n);

void rgba2rgb_avx2(uint8_t const *in_rgba, uint8_t *out_rgb, size_t n);
void rgba2rgb_default(uint8_t const *in_rgba, uint8_t *out_rgb, size_t n);

typedef void rgba2rgb_t(uint8_t const *in_rgba, uint8_t *out_rgb, size_t n);

extern "C" rgba2rgb_t *rgba2rgb_dispatch() {
    if (__builtin_cpu_supports("avx2")) {
        return &rgba2rgb_avx2;
    } else {
        return &rgba2rgb_default;
    }
}

void rgba2rgb_avx2(uint8_t const *in_rgba, uint8_t *out_rgb, size_t n) {
    const __m128i shuf1 = _mm_setr_epi8(0,1,2,4,5,6,8,9,10,12,13,14,3,7,11,15);
    const __m128i shuf2 = _mm_setr_epi8(5,6,8,9,10,12,13,14,3,7,11,15,0,1,2,4);
    const __m128i shuf3 = _mm_setr_epi8(10,12,13,14,3,7,11,15,0,1,2,4,5,6,8,9);
    const __m128i shuf4 = _mm_setr_epi8(3,7,11,15,0,1,2,4,5,6,8,9,10,12,13,14);
    auto in_rgba_end = in_rgba + ((n - 16) / 16 * 16) * 4;
    auto in_rgba_true_end = in_rgba + n * 4;
    while (in_rgba < in_rgba_end) {
        __m128i v1_rgba = _mm_loadu_si128((__m128i *)in_rgba); in_rgba += 16;
        __m128i v2_rgba = _mm_loadu_si128((__m128i *)in_rgba); in_rgba += 16;
        __m128i v3_rgba = _mm_loadu_si128((__m128i *)in_rgba); in_rgba += 16;
        __m128i v4_rgba = _mm_loadu_si128((__m128i *)in_rgba); in_rgba += 16;
        __m128i v1_rgb = _mm_shuffle_epi8(v1_rgba, shuf1);
        __m128i v2_rgb = _mm_shuffle_epi8(v2_rgba, shuf2);
        __m128i v3_rgb = _mm_shuffle_epi8(v3_rgba, shuf3);
        __m128i v4_rgb = _mm_shuffle_epi8(v4_rgba, shuf4);
        __m128i v1e_rgb = _mm_blend_epi32(v1_rgb, v2_rgb, 0b1000);
        __m128i v2e_rgb = _mm_blend_epi32(v2_rgb, v3_rgb, 0b1100);
        __m128i v3e_rgb = _mm_blend_epi32(v3_rgb, v4_rgb, 0b1110);
        _mm_storeu_si128((__m128i *)out_rgb, v1e_rgb); out_rgb += 16;
        _mm_storeu_si128((__m128i *)out_rgb, v2e_rgb); out_rgb += 16;
        _mm_storeu_si128((__m128i *)out_rgb, v3e_rgb); out_rgb += 16;
    }
    while (in_rgba != in_rgba_true_end) {
        *out_rgb++ = *in_rgba++;
        *out_rgb++ = *in_rgba++;
        *out_rgb++ = *in_rgba++;
        in_rgba++;
    }
}

void rgba2rgb_default(uint8_t const *in_rgba, uint8_t *out_rgb, size_t n) {
    for (size_t i = 0; i < n; i++) {
        out_rgb[i * 3 + 0] = in_rgba[i * 4 + 0];
        out_rgb[i * 3 + 1] = in_rgba[i * 4 + 1];
        out_rgb[i * 3 + 2] = in_rgba[i * 4 + 2];
    }
}
```

注意：MSVC 不支持动态分发，只能编译期分发，动态分发是 GCC 和 Clang 才有的特性。
