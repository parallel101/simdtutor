#pragma once

#include "dispatch_kernel.h"

DISPATCH_KERNEL_BEGIN(void, kernel_u8rgba2rgb, (uint8_t const *RESTRICT in_rgba, uint8_t *RESTRICT out_rgb, size_t n) {
    for (size_t i = 0; i < n; i++) {
        out_rgb[i * 3 + 0] = in_rgba[i * 4 + 0];
        out_rgb[i * 3 + 1] = in_rgba[i * 4 + 1];
        out_rgb[i * 3 + 2] = in_rgba[i * 4 + 2];
    }
});

DISPATCH_KERNEL("sse4.1", void, kernel_u8rgba2rgb, (uint8_t const *RESTRICT in_rgba, uint8_t *RESTRICT out_rgb, size_t n) {
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
});

DISPATCH_KERNEL("avx2", void, kernel_u8rgba2rgb, (uint8_t const *RESTRICT in_rgba, uint8_t *RESTRICT out_rgb, size_t n) {
    __m256i shuf12 = _mm256_setr_epi8(0,1,2,4,5,6,8,9,10,12,13,14,3,7,11,15,
                                      5,6,8,9,10,12,13,14,3,7,11,15,0,1,2,4);
    __m256i shuf34 = _mm256_setr_epi8(10,12,13,14,3,7,11,15,0,1,2,4,5,6,8,9,
                                      3,7,11,15,0,1,2,4,5,6,8,9,10,12,13,14);
    __m256i perm12 = _mm256_setr_epi32(0, 1, 2, 7, 4, 5, 3, 6);
    __m256i perm34 = _mm256_setr_epi32(0, 5, 6, 7, 1, 4, 2, 3);
    auto in_rgba_end = in_rgba + ((n - 16) / 16 * 16) * 4;
    auto in_rgba_true_end = in_rgba + n * 4;
    while (in_rgba < in_rgba_end) {
        __m256i v12_rgba = _mm256_loadu_si256((__m256i *)in_rgba);
        in_rgba += 32;
        __m256i v34_rgba = _mm256_loadu_si256((__m256i *)in_rgba);
        in_rgba += 32;
        __m256i v12_rgb = _mm256_shuffle_epi8(v12_rgba, shuf12);
        __m256i v34_rgb = _mm256_shuffle_epi8(v34_rgba, shuf34);
        __m256i v12t_rgb = _mm256_permutevar8x32_epi32(v12_rgb, perm12);
        __m256i v34t_rgb = _mm256_permutevar8x32_epi32(v34_rgb, perm34);
        __m256i v12e_rgb = _mm256_blend_epi32(v12t_rgb, v34t_rgb, 0b11000000);
        __m128i v3e_rgb = _mm256_castsi256_si128(v34t_rgb);
        _mm256_storeu_si256((__m256i *)out_rgb, v12e_rgb);
        out_rgb += 32;
        _mm_storeu_si128((__m128i *)out_rgb, v3e_rgb);
        out_rgb += 16;
    }

    while (in_rgba != in_rgba_true_end) {
        *out_rgb++ = *in_rgba++;
        *out_rgb++ = *in_rgba++;
        *out_rgb++ = *in_rgba++;
        in_rgba++;
    }
});

DISPATCH_KERNEL_END(void, kernel_u8rgba2rgb, (uint8_t const *RESTRICT in_rgba, uint8_t *RESTRICT out_rgb, size_t n), {n * 4, n * 3, n}, bpi(7));
