# rgba2rgb 最佳实践

- 输入: rgbargbargbargba rgbargbargbargba rgbargbargbargba rgbargbargbargba (64 * uint8)
- 输出: rgbrgbrgbrgbrgbr gbrgbrgbrgbrgbrg brgbrgbrgbrgbrgb (48 * uint8)

核心代码：从 4x4 矩阵到 4x3 矩阵的压缩。

```cpp
const __m128i shuf1 = _mm_setr_epi8(0,1,2,4,5,6,8,9,10,12,13,14,3,7,11,15);
const __m128i shuf2 = _mm_setr_epi8(5,6,8,9,10,12,13,14,3,7,11,15,0,1,2,4);
const __m128i shuf3 = _mm_setr_epi8(10,12,13,14,3,7,11,15,0,1,2,4,5,6,8,9);
const __m128i shuf4 = _mm_setr_epi8(3,7,11,15,0,1,2,4,5,6,8,9,10,12,13,14);
for (...) {
    __m128i v1_rgba = _mm_loadu_si128((__m128i_u *)in_rgba); in_rgba += 16;
    __m128i v2_rgba = _mm_loadu_si128((__m128i_u *)in_rgba); in_rgba += 16;
    __m128i v3_rgba = _mm_loadu_si128((__m128i_u *)in_rgba); in_rgba += 16;
    __m128i v4_rgba = _mm_loadu_si128((__m128i_u *)in_rgba); in_rgba += 16;
    // 核心代码开始
    __m128i v1_rgb = _mm_shuffle_epi8(v1_rgba, shuf1);
    __m128i v2_rgb = _mm_shuffle_epi8(v2_rgba, shuf2);
    __m128i v3_rgb = _mm_shuffle_epi8(v3_rgba, shuf3);
    __m128i v4_rgb = _mm_shuffle_epi8(v4_rgba, shuf4);
    __m128i v1e_rgb = _mm_blend_epi32(v1_rgb, v2_rgb, 0b1000);
    __m128i v2e_rgb = _mm_blend_epi32(v2_rgb, v3_rgb, 0b1100);
    __m128i v3e_rgb = _mm_blend_epi32(v3_rgb, v4_rgb, 0b1110);
    // 核心代码结束
    _mm_storeu_si128((__m128i_u *)out_rgb, v1e_rgb); out_rgb += 16;
    _mm_storeu_si128((__m128i_u *)out_rgb, v2e_rgb); out_rgb += 16;
    _mm_storeu_si128((__m128i_u *)out_rgb, v3e_rgb); out_rgb += 16;
}
```
