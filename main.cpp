#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include <random>
#include <memory>
#include <cstddef>
#include <immintrin.h>

// BEGIN CODE
void rgba2rgb(uint8_t const *in_rgba, uint8_t *out_rgb, size_t n) {
    __m256i shuf12 = _mm256_setr_epi8(0,1,2,4,5,6,8,9,10,12,13,14,3,7,11,15,
                                   5,6,8,9,10,12,13,14,3,7,11,15,0,1,2,4);
    __m256i shuf34 = _mm256_setr_epi8(10,12,13,14,3,7,11,15,0,1,2,4,5,6,8,9,
                                   3,7,11,15,0,1,2,4,5,6,8,9,10,12,13,14);
    __m256i perm12 = _mm256_setr_epi32(0, 1, 2, 7, 4, 5, 3, 6);
    __m256i perm34 = _mm256_setr_epi32(0, 5, 6, 7, 1, 4, 2, 3);
    auto in_rgba_end = in_rgba + ((n - 16) / 16 * 16) * 4;
    auto in_rgba_true_end = in_rgba + n * 4;
    while (in_rgba < in_rgba_end) {
        // rgbargbargbargba RGBaRgbargbargba rgbargbargBaRGBa rgbargbargbargba
        // rgbrgbrgbrgb.... gbrgbrgb    RGBR BRGB    rgbrgbrg ....rgbrgbrgbrgb
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
