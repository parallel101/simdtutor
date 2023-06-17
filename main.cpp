#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include <random>
#include <memory>
#include <cstddef>
#include <immintrin.h>

static __m256i masklut[512];

static int _init_lut = [] {
    for (int i = 0; i < 256; i++) {
        int per[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        for (int j = 0, k = 0; k < 8; k++) {
            if (i & (1 << k)) {
                per[j++] = k;
            }
        }
        __m256i perm = _mm256_loadu_si256((const __m256i *)per);
        _mm256_store_si256(masklut + i * 2, perm);
        int c = _mm_popcnt_u32(i);
        __m256i mask = _mm256_setr_epi32(
            c > 0 ? -1 : 0,
            c > 1 ? -1 : 0,
            c > 2 ? -1 : 0,
            c > 3 ? -1 : 0,
            c > 4 ? -1 : 0,
            c > 5 ? -1 : 0,
            c > 6 ? -1 : 0,
            c > 7 ? -1 : 0);
        _mm256_store_si256(masklut + i * 2 + 1, mask);
    }
    return 0;
} ();

// BEGIN CODE
size_t filterp(float const *x, size_t n, float y, float *z) {
    size_t j = 0;
    for (size_t i = 0; i < n; i++) {
        if (x[i] > y)
            z[j++] = x[i];
    }
    return j;
}
// END CODE

static void bench(benchmark::State &s) {
    const auto n = size_t(8192);
    auto x = std::vector<float>(n);
    auto z = std::vector<float>(n);
    auto y = 0.5f;
    std::generate(x.begin(), x.end(), [uni = std::uniform_real_distribution<float>(), rng = std::mt19937()] () mutable { return uni(rng); });
    for (auto _: s) {
        auto ret = filterp(x.data(), n, y, z.data());
        benchmark::DoNotOptimize(x);
        benchmark::DoNotOptimize(z);
        benchmark::DoNotOptimize(ret);
    }
    s.SetItemsProcessed(n * s.iterations());
}
BENCHMARK(bench);

size_t scalar_filterp(float const *x, size_t n, float y, float *z) {
    size_t j = 0;
    for (size_t i = 0; i < n; i++) {
        if (x[i] > y)
            z[j++] = x[i];
    }
    return j;
}

TEST(MySuite, MyTest) {
    const auto n = size_t(16384);
    auto x = std::vector<float>(n);
    auto z = std::vector<float>(n);
    auto z2 = std::vector<float>(n);
    auto y = 0.5f;
    std::generate(x.begin(), x.end(), [uni = std::uniform_real_distribution<float>(), rng = std::mt19937()] () mutable { return uni(rng); });
    auto ret = filterp(x.data(), n, y, z.data());
    auto ret2 = scalar_filterp(x.data(), n, y, z2.data());
    EXPECT_EQ(ret, ret2);
    if (ret == ret2) {
        for (size_t i = 0; i < ret; i++) {
            EXPECT_EQ(z[i], z2[i]);
            if (z[i] != z2[i]) break;
        }
    }
}
