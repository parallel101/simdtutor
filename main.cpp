#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include <random>
#include <memory>
#include <cstddef>
#include <immintrin.h>

// BEGIN CODE
void rgba2rgb(uint8_t const *in_rgba, uint8_t *out_rgb, size_t n) {
    __m128i shuf1 = _mm_setr_epi8(0,1,2, 4,5,6, 8,9,10, 12,13,14, 3,7,11,15);
    __m128i shuf2 = _mm_setr_epi8(5,6, 8,9,10, 12,13,14, 3,7,11,15, 0,1,2,4);
    __m128i shuf3 = _mm_setr_epi8(10,12,13,14, 3,7,11,15, 0,1,2, 4,5,6, 8,9);
    __m128i shuf4 = _mm_setr_epi8(3,7,11,15, 0,1,2, 4,5,6, 8,9,10, 12,13,14);
    auto in_rgba_end = in_rgba + ((n - 16) / 16 * 16) * 4;
    auto in_rgba_true_end = in_rgba + n * 4;
    while (in_rgba < in_rgba_end) {
        // rgbargbargbargba RGBaRgbargbargba rgbargbargBaRGBa rgbargbargbargba
        // rgbrgbrgbrgb.... gbrgbrgb    RGBR BRGB    rgbrgbrg ....rgbrgbrgbrgb
        __m128i v1_rgba = _mm_loadu_si128((__m128i *)in_rgba);
        in_rgba += 16;
        __m128i v2_rgba = _mm_loadu_si128((__m128i *)in_rgba);
        in_rgba += 16;
        __m128i v3_rgba = _mm_loadu_si128((__m128i *)in_rgba);
        in_rgba += 16;
        __m128i v4_rgba = _mm_loadu_si128((__m128i *)in_rgba);
        in_rgba += 16;
        __m128i v1_rgb = _mm_shuffle_epi8(v1_rgba, shuf1);
        __m128i v2_rgb = _mm_shuffle_epi8(v2_rgba, shuf2);
        __m128i v3_rgb = _mm_shuffle_epi8(v3_rgba, shuf3);
        __m128i v4_rgb = _mm_shuffle_epi8(v4_rgba, shuf4);
        __m128i v1e_rgb = _mm_blend_epi32(v1_rgb, v2_rgb, 0b1000);
        __m128i v2e_rgb = _mm_blend_epi32(v2_rgb, v3_rgb, 0b1100);
        __m128i v3e_rgb = _mm_blend_epi32(v3_rgb, v4_rgb, 0b1110);
        _mm_storeu_si128((__m128i *)out_rgb, v1e_rgb);
        out_rgb += 16;
        _mm_storeu_si128((__m128i *)out_rgb, v2e_rgb);
        out_rgb += 16;
        _mm_storeu_si128((__m128i *)out_rgb, v3e_rgb);
        out_rgb += 16;
    }

    while (in_rgba != in_rgba_true_end) {
        *out_rgb++ = *in_rgba++;
        *out_rgb++ = *in_rgba++;
        *out_rgb++ = *in_rgba++;
        in_rgba++;
    }
}
// END CODE

static void bench(benchmark::State &s) {
    const auto n = size_t(2073600);
    auto img = std::vector<uint8_t>(n * 4);
    auto out = std::vector<uint8_t>(n * 3);
    std::generate(img.begin(), img.end(), [uni = std::uniform_int_distribution<uint8_t>(), rng = std::mt19937()] () mutable { return uni(rng); });
    for (auto _: s) {
        rgba2rgb(img.data(), out.data(), n);
        benchmark::DoNotOptimize(img);
        benchmark::DoNotOptimize(out);
    }
    s.SetItemsProcessed(n * s.iterations());
    s.SetBytesProcessed(n * 7 * s.iterations());
}
BENCHMARK(bench);

void scalar_rgba2rgb(uint8_t const *in_rgba, uint8_t *out_rgb, size_t n) {
    for (size_t i = 0; i < n; i++) {
        out_rgb[i * 3 + 0] = in_rgba[i * 4 + 0];
        out_rgb[i * 3 + 1] = in_rgba[i * 4 + 1];
        out_rgb[i * 3 + 2] = in_rgba[i * 4 + 2];
    }
}

TEST(MySuite, MyTest) {
    const auto n = size_t(2073600);
    auto img = std::vector<uint8_t>(n * 4);
    auto out = std::vector<uint8_t>(n * 3);
    auto out2 = std::vector<uint8_t>(n * 3);
    std::generate(img.begin(), img.end(), [uni = std::uniform_int_distribution<uint8_t>(), rng = std::mt19937()] () mutable { return uni(rng); });
    rgba2rgb(img.data(), out.data(), n);
    scalar_rgba2rgb(img.data(), out2.data(), n);
    for (size_t i = 0; i < n * 3; i++) {
        EXPECT_EQ(out[i], out2[i]);
        if (out[i] != out2[i]) break;
    }
}
