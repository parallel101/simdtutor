#pragma once

#include "dispatch_kernel.h"
#include <random>
#include <limits>
#include <cstring>
#include <array>

DISPATCH_KERNEL_BEGIN(void, kernel_u8mt19937rng, (uint8_t *RESTRICT out, size_t n, uint32_t seed) {
    auto uni = std::uniform_int_distribution<uint8_t>();
    auto rng = std::mt19937(seed);
    for (size_t i = 0; i < n; i++) {
        out[i] = uni(rng);
    }
});

DISPATCH_KERNEL_END(void, kernel_u8mt19937rng, (uint8_t *RESTRICT out, size_t n, uint32_t seed), {n, n, 42}, bpi(1));

DISPATCH_KERNEL_BEGIN(void, kernel_f32wangshashrng, (float *RESTRICT out, size_t n, float bot, float top, uint32_t base, uint32_t seed) {
    float k = ((top - bot) * (1.0f / (float)(1 << 25)));
    for (size_t i = 0; i < n; i++) {
        auto x = ((uint32_t)i + base) ^ seed;
        x = (x ^ 61) ^ (x >> 16);
        x *= 9;
        x = x ^ (x >> 4);
        x *= 0x27d4eb2d;
        x = x ^ (x >> 15);
        out[i] = (float)(x >> 7) * k + bot;
    }
});

DISPATCH_KERNEL("sse4.1", void, kernel_f32wangshashrng, (float *RESTRICT out, size_t n, float bot, float top, uint32_t base, uint32_t seed) {
    auto out_end = out + (n >> 2 << 2);
    auto out_true_end = out + n;
    auto i = _mm_setr_epi32(0,1,2,3);
    auto d = _mm_set1_epi32(4);
    auto s = _mm_set1_epi32(seed);
    auto c1 = _mm_set1_epi32(61);
    auto c2 = _mm_set1_epi32(0x27d4eb2d);
    auto k = _mm_set1_ps((top - bot) * (1.0f / (float)(1 << 25)));
    auto b = _mm_set1_ps(bot);
    i = _mm_add_epi32(i, _mm_set1_epi32(base));
    while (out != out_end) {
        auto x = _mm_xor_si128(i, s);
        i = _mm_add_epi32(i, d);
        x = _mm_xor_si128(_mm_xor_si128(x, c1), _mm_srli_epi32(x, 16));
        x = _mm_add_epi32(x, _mm_slli_epi32(x, 3));
        x = _mm_xor_si128(x, _mm_srli_epi32(x, 4));
        x = _mm_mullo_epi32(x, c2);
        x = _mm_xor_si128(x, _mm_srli_epi32(x, 15));
        auto xf = _mm_add_ps(b, _mm_mul_ps(_mm_cvtepi32_ps(_mm_srli_epi32(x, 7)), k));
        _mm_storeu_ps(out, xf);
        out += 4;
    }
    auto idx = (n >> 2 << 2);
    while (out != out_true_end) {
        auto x = ((uint32_t)idx++ + base) ^ seed;
        x = (x ^ 61) ^ (x >> 16);
        x *= 9;
        x = x ^ (x >> 4);
        x *= 0x27d4eb2d;
        x = x ^ (x >> 15);
        *out++ = (float)(x >> 7) * _mm_cvtss_f32(k) + bot;
    }
});

DISPATCH_KERNEL("sse4.1,fma", void, kernel_f32wangshashrng, (float *RESTRICT out, size_t n, float bot, float top, uint32_t base, uint32_t seed) {
    auto out_end = out + (n >> 2 << 2);
    auto out_true_end = out + n;
    auto i = _mm_setr_epi32(0,1,2,3);
    auto d = _mm_set1_epi32(4);
    auto s = _mm_set1_epi32(seed);
    auto c1 = _mm_set1_epi32(61);
    auto c2 = _mm_set1_epi32(0x27d4eb2d);
    auto k = _mm_set1_ps((top - bot) * (1.0f / (float)(1 << 25)));
    auto b = _mm_set1_ps(bot);
    i = _mm_add_epi32(i, _mm_set1_epi32(base));
    while (out != out_end) {
        auto x = _mm_xor_si128(i, s);
        i = _mm_add_epi32(i, d);
        x = _mm_xor_si128(_mm_xor_si128(x, c1), _mm_srli_epi32(x, 16));
        x = _mm_add_epi32(x, _mm_slli_epi32(x, 3));
        x = _mm_xor_si128(x, _mm_srli_epi32(x, 4));
        x = _mm_mullo_epi32(x, c2);
        x = _mm_xor_si128(x, _mm_srli_epi32(x, 15));
        auto xf = _mm_fmadd_ps(_mm_cvtepi32_ps(_mm_srli_epi32(x, 7)), k, b);
        _mm_storeu_ps(out, xf);
        out += 4;
    }
    auto idx = (n >> 2 << 2);
    while (out != out_true_end) {
        auto x = ((uint32_t)idx++ + base) ^ seed;
        x = (x ^ 61) ^ (x >> 16);
        x *= 9;
        x = x ^ (x >> 4);
        x *= 0x27d4eb2d;
        x = x ^ (x >> 15);
        *out++ = (float)(x >> 7) * _mm_cvtss_f32(k) + bot;
    }
});

DISPATCH_KERNEL("avx2", void, kernel_f32wangshashrng, (float *RESTRICT out, size_t n, float bot, float top, uint32_t base, uint32_t seed) {
    auto out_end = out + (n >> 3 << 3);
    auto out_true_end = out + n;
    auto i = _mm256_setr_epi32(0,1,2,3,4,5,6,7);
    auto d = _mm256_set1_epi32(8);
    auto s = _mm256_set1_epi32(seed);
    auto c1 = _mm256_set1_epi32(61);
    auto c2 = _mm256_set1_epi32(0x27d4eb2d);
    auto k = _mm256_set1_ps((top - bot) * (1.0f / (float)(1 << 25)));
    auto b = _mm256_set1_ps(bot);
    i = _mm256_add_epi32(i, _mm256_set1_epi32(base));
    while (out != out_end) {
        auto x = _mm256_xor_si256(i, s);
        i = _mm256_add_epi32(i, d);
        x = _mm256_xor_si256(_mm256_xor_si256(x, c1), _mm256_srli_epi32(x, 16));
        x = _mm256_add_epi32(x, _mm256_slli_epi32(x, 3));
        x = _mm256_xor_si256(x, _mm256_srli_epi32(x, 4));
        x = _mm256_mullo_epi32(x, c2);
        x = _mm256_xor_si256(x, _mm256_srli_epi32(x, 15));
        auto xf = _mm256_add_ps(b, _mm256_mul_ps(_mm256_cvtepi32_ps(_mm256_srli_epi32(x, 7)), k));
        _mm256_storeu_ps(out, xf);
        out += 8;
    }
    auto idx = (n >> 3 << 3);
    while (out != out_true_end) {
        auto x = ((uint32_t)idx++ + base) ^ seed;
        x = (x ^ 61) ^ (x >> 16);
        x *= 9;
        x = x ^ (x >> 4);
        x *= 0x27d4eb2d;
        x = x ^ (x >> 15);
        *out++ = (float)(x >> 7) * _mm256_cvtss_f32(k) + bot;
    }
});

DISPATCH_KERNEL("avx2,fma", void, kernel_f32wangshashrng, (float *RESTRICT out, size_t n, float bot, float top, uint32_t base, uint32_t seed) {
    auto out_end = out + (n >> 3 << 3);
    auto out_true_end = out + n;
    auto i = _mm256_setr_epi32(0,1,2,3,4,5,6,7);
    auto d = _mm256_set1_epi32(8);
    auto s = _mm256_set1_epi32(seed);
    auto c1 = _mm256_set1_epi32(61);
    auto c2 = _mm256_set1_epi32(0x27d4eb2d);
    auto k = _mm256_set1_ps((top - bot) * (1.0f / (float)(1 << 25)));
    auto b = _mm256_set1_ps(bot);
    i = _mm256_add_epi32(i, _mm256_set1_epi32(base));
    while (out != out_end) {
        auto x = _mm256_xor_si256(i, s);
        i = _mm256_add_epi32(i, d);
        x = _mm256_xor_si256(_mm256_xor_si256(x, c1), _mm256_srli_epi32(x, 16));
        x = _mm256_add_epi32(x, _mm256_slli_epi32(x, 3));
        x = _mm256_xor_si256(x, _mm256_srli_epi32(x, 4));
        x = _mm256_mullo_epi32(x, c2);
        x = _mm256_xor_si256(x, _mm256_srli_epi32(x, 15));
        auto xf = _mm256_fmadd_ps(_mm256_cvtepi32_ps(_mm256_srli_epi32(x, 7)), k, b);
        _mm256_storeu_ps(out, xf);
        out += 8;
    }
    auto idx = (n >> 3 << 3);
    while (out != out_true_end) {
        auto x = ((uint32_t)idx++ + base) ^ seed;
        x = (x ^ 61) ^ (x >> 16);
        x *= 9;
        x = x ^ (x >> 4);
        x *= 0x27d4eb2d;
        x = x ^ (x >> 15);
        *out++ = (float)(x >> 7) * _mm256_cvtss_f32(k) + bot;
    }
});

DISPATCH_KERNEL_END(void, kernel_f32wangshashrng, (float *RESTRICT out, size_t n, float bot, float top, uint32_t base, uint32_t seed), {n, n, 2.718f, 3.14f, 0, 42}, bpi(4));

DISPATCH_KERNEL_BEGIN(void, kernel_u32wangshashrng, (uint32_t *RESTRICT out, size_t n, uint32_t base, uint32_t seed) {
    for (size_t i = 0; i < n; i++) {
        auto x = ((uint32_t)i + base) ^ seed;
        x = (x ^ 61) ^ (x >> 16);
        x *= 9;
        x = x ^ (x >> 4);
        x *= 0x27d4eb2d;
        x = x ^ (x >> 15);
        out[i] = x;
    }
});

DISPATCH_KERNEL("sse4.1", void, kernel_u32wangshashrng, (uint32_t *RESTRICT out, size_t n, uint32_t base, uint32_t seed) {
    auto out_end = out + (n >> 2 << 2);
    auto out_true_end = out + n;
    auto i = _mm_setr_epi32(0,1,2,3);
    auto d = _mm_set1_epi32(4);
    auto s = _mm_set1_epi32(seed);
    auto c1 = _mm_set1_epi32(61);
    auto c2 = _mm_set1_epi32(0x27d4eb2d);
    i = _mm_add_epi32(i, _mm_set1_epi32(base));
    while (out != out_end) {
        auto x = _mm_xor_si128(i, s);
        i = _mm_add_epi32(i, d);
        x = _mm_xor_si128(_mm_xor_si128(x, c1), _mm_srli_epi32(x, 16));
        x = _mm_add_epi32(x, _mm_slli_epi32(x, 3));
        x = _mm_xor_si128(x, _mm_srli_epi32(x, 4));
        x = _mm_mullo_epi32(x, c2);
        x = _mm_xor_si128(x, _mm_srli_epi32(x, 15));
        _mm_storeu_si128((__m128i *)out, x);
        out += 4;
    }
    auto idx = (n >> 2 << 2);
    while (out != out_true_end) {
        auto x = ((uint32_t)idx++ + base) ^ seed;
        x = (x ^ 61) ^ (x >> 16);
        x *= 9;
        x = x ^ (x >> 4);
        x *= 0x27d4eb2d;
        x = x ^ (x >> 15);
        *out++ = x;
    }
});

DISPATCH_KERNEL("avx2", void, kernel_u32wangshashrng, (uint32_t *RESTRICT out, size_t n, uint32_t base, uint32_t seed) {
    auto out_end = out + (n >> 3 << 3);
    auto out_true_end = out + n;
    auto i = _mm256_setr_epi32(0,1,2,3,4,5,6,7);
    auto d = _mm256_set1_epi32(8);
    auto s = _mm256_set1_epi32(seed);
    auto c1 = _mm256_set1_epi32(61);
    auto c2 = _mm256_set1_epi32(0x27d4eb2d);
    i = _mm256_add_epi32(i, _mm256_set1_epi32(base));
    while (out != out_end) {
        auto x = _mm256_xor_si256(i, s);
        i = _mm256_add_epi32(i, d);
        x = _mm256_xor_si256(_mm256_xor_si256(x, c1), _mm256_srli_epi32(x, 16));
        x = _mm256_add_epi32(x, _mm256_slli_epi32(x, 3));
        x = _mm256_xor_si256(x, _mm256_srli_epi32(x, 4));
        x = _mm256_mullo_epi32(x, c2);
        x = _mm256_xor_si256(x, _mm256_srli_epi32(x, 15));
        _mm256_storeu_si256((__m256i *)out, x);
        out += 8;
    }
    auto idx = (n >> 3 << 3);
    while (out != out_true_end) {
        auto x = ((uint32_t)idx++ + base) ^ seed;
        x = (x ^ 61) ^ (x >> 16);
        x *= 9;
        x = x ^ (x >> 4);
        x *= 0x27d4eb2d;
        x = x ^ (x >> 15);
        *out++ = x;
    }
});

DISPATCH_KERNEL_END(void, kernel_u32wangshashrng, (uint32_t *RESTRICT out, size_t n, uint32_t base, uint32_t seed), {n, n, 0, 42}, bpi(4));

DISPATCH_KERNEL_BEGIN(void, kernel_u8wangshashrng, (uint8_t *RESTRICT out, size_t n, uint32_t base, uint32_t seed) {
    for (size_t i = 0; i < n; i++) {
        auto x = ((uint32_t)i + base) ^ seed;
        x = (x ^ 61) ^ (x >> 16);
        x *= 9;
        x = x ^ (x >> 4);
        x *= 0x27d4eb2d;
        x = x ^ (x >> 15);
        out[i] = x & 0xff;
    }
});

DISPATCH_KERNEL("avx2", void, kernel_u8wangshashrng, (uint8_t *RESTRICT out, size_t n, uint32_t base, uint32_t seed) {
    auto out_end = out + (n >> 4 << 4);
    auto out_true_end = out + n;
    auto i = _mm256_setr_epi32(0,1,2,3,4,5,6,7);
    auto d = _mm256_set1_epi32(8);
    auto s = _mm256_set1_epi32(seed);
    auto c1 = _mm256_set1_epi32(61);
    auto c2 = _mm256_set1_epi32(0x27d4eb2d);
    i = _mm256_add_epi32(i, _mm256_set1_epi32(base));
    auto shuf = _mm256_setr_epi8(0,4,8,12,4,0,0,0,1,0,0,0,5,0,0,0,
                                 0,4,8,12,0,0,0,0,0,0,0,0,0,0,0,0);
    while (out != out_end) {
        auto x = _mm256_xor_si256(i, s);
        i = _mm256_add_epi32(i, d);
        x = _mm256_xor_si256(_mm256_xor_si256(x, c1), _mm256_srli_epi32(x, 16));
        x = _mm256_add_epi32(x, _mm256_slli_epi32(x, 3));
        x = _mm256_xor_si256(x, _mm256_srli_epi32(x, 4));
        x = _mm256_mullo_epi32(x, c2);
        x = _mm256_xor_si256(x, _mm256_srli_epi32(x, 15));
        x = _mm256_shuffle_epi8(x, shuf);
        auto y = _mm256_xor_si256(i, s);
        i = _mm256_add_epi32(i, d);
        y = _mm256_xor_si256(_mm256_xor_si256(y, c1), _mm256_srli_epi32(y, 16));
        y = _mm256_mullo_epi32(y, c2);
        y = _mm256_xor_si256(y, _mm256_srli_epi32(y, 4));
        y = _mm256_mullo_epi32(y, c2);
        y = _mm256_xor_si256(y, _mm256_srli_epi32(y, 15));
        y = _mm256_shuffle_epi8(y, shuf);
        auto xy = _mm256_unpacklo_epi32(x, y);
        xy = _mm256_permutevar8x32_epi32(xy, shuf);
        _mm_storeu_si128((__m128i *)out, _mm256_castsi256_si128(xy));
        out += 16;
    }
    auto idx = n >> 4 << 4;
    while (out != out_true_end) {
        auto x = ((uint32_t)idx++ + base) ^ seed;
        x = (x ^ 61) ^ (x >> 16);
        x *= 9;
        x = x ^ (x >> 4);
        x *= 0x27d4eb2d;
        x = x ^ (x >> 15);
        *out++ = x & 0xff;
    }
});

DISPATCH_KERNEL_END(void, kernel_u8wangshashrng, (uint8_t *RESTRICT out, size_t n, uint32_t base, uint32_t seed), {n, n, 0, 42}, bpi(1));

DISPATCH_KERNEL_BEGIN(void, kernel_u8fastwangshashrng, (uint8_t *RESTRICT out, size_t n, uint32_t seed) {
    for (size_t i = 0; i < n; i++) {
        auto x = (uint32_t)i ^ seed;
        x = (x ^ 61) ^ (x >> 16);
        x *= 9;
        x = x ^ (x >> 4);
        x *= 0x27d4eb2d;
        x = x ^ (x >> 15);
        out[i] = x & 0xff;
    }
});

DISPATCH_KERNEL("sse4.1", void, kernel_u8fastwangshashrng, (uint8_t *RESTRICT out, size_t n, uint32_t seed) {
    auto out_end = out + (n >> 4 << 4);
    auto out_true_end = out + n;
    auto i = _mm_setr_epi32(0,1,2,3);
    auto d = _mm_set1_epi32(4);
    auto s = _mm_set1_epi32(seed);
    auto c1 = _mm_set1_epi32(61);
    auto c2 = _mm_set1_epi32(0x27d4eb2d);
    while (out != out_end) {
        auto x = _mm_xor_si128(i, s);
        i = _mm_add_epi32(i, d);
        x = _mm_xor_si128(_mm_xor_si128(x, c1), _mm_srli_epi32(x, 16));
        x = _mm_add_epi32(x, _mm_slli_epi32(x, 3));
        x = _mm_xor_si128(x, _mm_srli_epi32(x, 4));
        x = _mm_mullo_epi32(x, c2);
        x = _mm_xor_si128(x, _mm_srli_epi32(x, 15));
        _mm_storeu_si128((__m128i *)out, x);
        out += 16;
    }
    for (size_t i = (n >> 4 << 4); i < n; i++) {
        auto x = (uint32_t)i ^ seed;
        x = (x ^ 61) ^ (x >> 16);
        x *= 9;
        x = x ^ (x >> 4);
        x *= 0x27d4eb2d;
        x = x ^ (x >> 15);
        out[i] = x & 0xff;
    }
});

DISPATCH_KERNEL("avx2", void, kernel_u8fastwangshashrng, (uint8_t *RESTRICT out, size_t n, uint32_t seed) {
    auto out_end = out + (n >> 5 << 5);
    auto out_true_end = out + n;
    auto i = _mm256_setr_epi32(0,1,2,3,4,5,6,7);
    auto d = _mm256_set1_epi32(8);
    auto s = _mm256_set1_epi32(seed);
    auto c1 = _mm256_set1_epi32(61);
    auto c2 = _mm256_set1_epi32(0x27d4eb2d);
    while (out != out_end) {
        auto x = _mm256_xor_si256(i, s);
        i = _mm256_add_epi32(i, d);
        x = _mm256_xor_si256(_mm256_xor_si256(x, c1), _mm256_srli_epi32(x, 16));
        x = _mm256_add_epi32(x, _mm256_slli_epi32(x, 3));
        x = _mm256_xor_si256(x, _mm256_srli_epi32(x, 4));
        x = _mm256_mullo_epi32(x, c2);
        x = _mm256_xor_si256(x, _mm256_srli_epi32(x, 15));
        _mm256_storeu_si256((__m256i *)out, x);
        out += 32;
    }
    for (size_t i = (n >> 5 << 5); i < n; i++) {
        auto x = (uint32_t)i ^ seed;
        x = (x ^ 61) ^ (x >> 16);
        x *= 9;
        x = x ^ (x >> 4);
        x *= 0x27d4eb2d;
        x = x ^ (x >> 15);
        out[i] = x & 0xff;
    }
});

DISPATCH_KERNEL("avx512f", void, kernel_u8fastwangshashrng, (uint8_t *RESTRICT out, size_t n, uint32_t seed) {
    auto out_end = out + (n >> 6 << 6);
    auto out_true_end = out + n;
    auto i = _mm512_setr_epi32(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);
    auto d = _mm512_set1_epi32(16);
    auto s = _mm512_set1_epi32(seed);
    auto c1 = _mm512_set1_epi32(61);
    auto c2 = _mm512_set1_epi32(0x27d4eb2d);
    while (out != out_end) {
        auto x = _mm512_xor_si512(i, s);
        i = _mm512_add_epi32(i, d);
        x = _mm512_xor_si512(_mm512_xor_si512(x, c1), _mm512_srli_epi32(x, 16));
        x = _mm512_add_epi32(x, _mm512_slli_epi32(x, 3));
        x = _mm512_xor_si512(x, _mm512_srli_epi32(x, 4));
        x = _mm512_mullo_epi32(x, c2);
        x = _mm512_xor_si512(x, _mm512_srli_epi32(x, 15));
        _mm512_storeu_si512((__m512i *)out, x);
        out += 64;
    }
    for (size_t i = (n >> 6 << 6); i < n; i++) {
        auto x = (uint32_t)i ^ seed;
        x = (x ^ 61) ^ (x >> 16);
        x *= 9;
        x = x ^ (x >> 4);
        x *= 0x27d4eb2d;
        x = x ^ (x >> 15);
        out[i] = x & 0xff;
    }
});

DISPATCH_KERNEL_END(void, kernel_u8fastwangshashrng, (uint8_t *RESTRICT out, size_t n, uint32_t seed), {n, n, 42}, bpi(1), notest);

DISPATCH_KERNEL_BEGIN(void, kernel_u32wangshashxrng, (uint32_t *RESTRICT out, size_t n, uint32_t base, uint32_t seed) {
    for (size_t i = 0; i < n; i++) {
        auto x = ((uint32_t)i + base) ^ seed;
        x += ~(x << 15);
        x ^= (x >> 10);
        x += (x >> 3);
        x ^= (x >> 6);
        x += ~(x << 11);
        x ^= (x >> 16);
        out[i] = x;
    }
});

DISPATCH_KERNEL("sse2", void, kernel_u32wangshashxrng, (uint32_t *RESTRICT out, size_t n, uint32_t base, uint32_t seed) {
    auto out_end = out + (n >> 2 << 2);
    auto out_true_end = out + n;
    auto i = _mm_setr_epi32(0,1,2,3);
    auto d = _mm_set1_epi32(4);
    auto s = _mm_set1_epi32(seed);
    auto full = _mm_set1_epi32(-1);
    i = _mm_add_epi32(i, _mm_set1_epi32(base));
    while (out != out_end) {
        auto x = _mm_xor_si128(i, s);
        i = _mm_add_epi32(i, d);
        x = _mm_add_epi32(x, _mm_andnot_si128(_mm_slli_epi32(x, 15), full));
        x = _mm_xor_si128(x, _mm_srli_epi32(x, 10));
        x = _mm_add_epi32(x, _mm_srli_epi32(x, 3));
        x = _mm_xor_si128(x, _mm_srli_epi32(x, 6));
        x = _mm_add_epi32(x, _mm_andnot_si128(_mm_slli_epi32(x, 11), full));
        x = _mm_xor_si128(x, _mm_srli_epi32(x, 16));
        _mm_storeu_si128((__m128i *)out, x);
        out += 4;
    }
    auto idx = (n >> 2 << 2);
    while (out != out_true_end) {
        auto x = ((uint32_t)idx++ + base) ^ seed;
        x += ~(x << 15);
        x ^= (x >> 10);
        x += (x >> 3);
        x ^= (x >> 6);
        x += ~(x << 11);
        x ^= (x >> 16);
        *out++ = x;
    }
});

DISPATCH_KERNEL("avx2", void, kernel_u32wangshashxrng, (uint32_t *RESTRICT out, size_t n, uint32_t base, uint32_t seed) {
    auto out_end = out + (n >> 3 << 3);
    auto out_true_end = out + n;
    auto i = _mm256_setr_epi32(0,1,2,3,4,5,6,7);
    auto d = _mm256_set1_epi32(8);
    auto s = _mm256_set1_epi32(seed);
    auto full = _mm256_set1_epi32(-1);
    i = _mm256_add_epi32(i, _mm256_set1_epi32(base));
    while (out != out_end) {
        auto x = _mm256_xor_si256(i, s);
        i = _mm256_add_epi32(i, d);
        x = _mm256_add_epi32(x, _mm256_andnot_si256(_mm256_slli_epi32(x, 15), full));
        x = _mm256_xor_si256(x, _mm256_srli_epi32(x, 10));
        x = _mm256_add_epi32(x, _mm256_srli_epi32(x, 3));
        x = _mm256_xor_si256(x, _mm256_srli_epi32(x, 6));
        x = _mm256_add_epi32(x, _mm256_andnot_si256(_mm256_slli_epi32(x, 11), full));
        x = _mm256_xor_si256(x, _mm256_srli_epi32(x, 16));
        _mm256_storeu_si256((__m256i *)out, x);
        out += 8;
    }
    auto idx = (n >> 3 << 3);
    while (out != out_true_end) {
        auto x = ((uint32_t)idx++ + base) ^ seed;
        x += ~(x << 15);
        x ^= (x >> 10);
        x += (x >> 3);
        x ^= (x >> 6);
        x += ~(x << 11);
        x ^= (x >> 16);
        *out++ = x;
    }
});

DISPATCH_KERNEL_END(void, kernel_u32wangshashxrng, (uint32_t *RESTRICT out, size_t n, uint32_t base, uint32_t seed), {n, n, 0, 42}, bpi(4));

DISPATCH_KERNEL_BEGIN(void, kernel_u8fastwangshashxrng, (uint8_t *RESTRICT out, size_t n, uint32_t seed) {
    for (size_t i = 0; i < n; i++) {
        auto x = (uint32_t)i ^ seed;
        x += ~(x << 15);
        x ^= (x >> 10);
        x += (x >> 3);
        x ^= (x >> 6);
        x += ~(x << 11);
        x ^= (x >> 16);
        out[i] = x & 0xff;
    }
});

DISPATCH_KERNEL("sse2", void, kernel_u8fastwangshashxrng, (uint8_t *RESTRICT out, size_t n, uint32_t seed) {
    auto out_end = out + (n >> 4 << 4);
    auto out_true_end = out + n;
    auto i = _mm_setr_epi32(0,1,2,3);
    auto d = _mm_set1_epi32(4);
    auto s = _mm_set1_epi32(seed);
    auto full = _mm_set1_epi32(-1);
    while (out != out_end) {
        auto x = _mm_xor_si128(i, s);
        i = _mm_add_epi32(i, d);
        x = _mm_add_epi32(x, _mm_andnot_si128(_mm_slli_epi32(x, 15), full));
        x = _mm_xor_si128(x, _mm_srli_epi32(x, 10));
        x = _mm_add_epi32(x, _mm_srli_epi32(x, 3));
        x = _mm_xor_si128(x, _mm_srli_epi32(x, 6));
        x = _mm_add_epi32(x, _mm_andnot_si128(_mm_slli_epi32(x, 11), full));
        x = _mm_xor_si128(x, _mm_srli_epi32(x, 16));
        _mm_storeu_si128((__m128i *)out, x);
        out += 16;
    }
    for (size_t i = (n >> 4 << 4); i < n; i++) {
        auto x = (uint32_t)i ^ seed;
        x += ~(x << 15);
        x ^= (x >> 10);
        x += (x >> 3);
        x ^= (x >> 6);
        x += ~(x << 11);
        x ^= (x >> 16);
        out[i] = x & 0xff;
    }
});

DISPATCH_KERNEL("avx2", void, kernel_u8fastwangshashxrng, (uint8_t *RESTRICT out, size_t n, uint32_t seed) {
    auto out_end = out + (n >> 5 << 5);
    auto out_true_end = out + n;
    auto i = _mm256_setr_epi32(0,1,2,3,4,5,6,7);
    auto d = _mm256_set1_epi32(16);
    auto s = _mm256_set1_epi32(seed);
    auto full = _mm256_set1_epi32(-1);
    while (out != out_end) {
        auto x = _mm256_xor_si256(i, s);
        x = _mm256_add_epi32(x, _mm256_andnot_si256(_mm256_slli_epi32(x, 15), full));
        x = _mm256_xor_si256(x, _mm256_srli_epi32(x, 10));
        x = _mm256_add_epi32(x, _mm256_srli_epi32(x, 3));
        x = _mm256_xor_si256(x, _mm256_srli_epi32(x, 6));
        x = _mm256_add_epi32(x, _mm256_andnot_si256(_mm256_slli_epi32(x, 11), full));
        x = _mm256_xor_si256(x, _mm256_srli_epi32(x, 16));
        _mm256_storeu_si256((__m256i *)out, x);
        out += 32;
    }
    for (size_t i = (n >> 5 << 5); i < n; i++) {
        auto x = (uint32_t)i ^ seed;
        x += ~(x << 15);
        x ^= (x >> 10);
        x += (x >> 3);
        x ^= (x >> 6);
        x += ~(x << 11);
        x ^= (x >> 16);
        out[i] = x & 0xff;
    }
});

DISPATCH_KERNEL("avx512f", void, kernel_u8fastwangshashxrng, (uint8_t *RESTRICT out, size_t n, uint32_t seed) {
    auto out_end = out + (n >> 6 << 6);
    auto out_true_end = out + n;
    auto i = _mm512_setr_epi32(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);
    auto d = _mm512_set1_epi32(16);
    auto s = _mm512_set1_epi32(seed);
    auto full = _mm512_set1_epi32(-1);
    while (out != out_end) {
        auto x = _mm512_xor_si512(i, s);
        x = _mm512_add_epi32(x, _mm512_andnot_si512(_mm512_slli_epi32(x, 15), full));
        x = _mm512_xor_si512(x, _mm512_srli_epi32(x, 10));
        x = _mm512_add_epi32(x, _mm512_srli_epi32(x, 3));
        x = _mm512_xor_si512(x, _mm512_srli_epi32(x, 6));
        x = _mm512_add_epi32(x, _mm512_andnot_si512(_mm512_slli_epi32(x, 11), full));
        x = _mm512_xor_si512(x, _mm512_srli_epi32(x, 16));
        _mm512_storeu_si512((__m512i *)out, x);
        out += 64;
    }
    for (size_t i = (n >> 6 << 6); i < n; i++) {
        auto x = (uint32_t)i ^ seed;
        x += ~(x << 15);
        x ^= (x >> 10);
        x += (x >> 3);
        x ^= (x >> 6);
        x += ~(x << 11);
        x ^= (x >> 16);
        out[i] = x & 0xff;
    }
});

DISPATCH_KERNEL_END(void, kernel_u8fastwangshashxrng, (uint8_t *RESTRICT out, size_t n, uint32_t seed), {n, n, 42}, bpi(1), notest);

// recommened: kernel_u8fastwangshashrng or kernel_f32wangshashrng
