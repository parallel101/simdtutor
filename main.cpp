#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include <random>
#include <memory>
#include <cstddef>
#include <immintrin.h>

__m256i masklut[512];

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
    __m256 pred = _mm256_set1_ps(y);
    auto zbeg = z;
    auto xend = x + n;
    while (x + 16 <= xend) {
        __m256 xi = _mm256_loadu_ps(x);
        __m256 mask = _mm256_cmp_ps(xi, pred, _CMP_GT_OS);
        __m256 xi2 = _mm256_loadu_ps(x + 8);
        x += 16;
        __m256 mask2 = _mm256_cmp_ps(xi2, pred, _CMP_GT_OS);
        size_t m = (size_t)_mm256_movemask_ps(mask) << 6;
        size_t m2 = (size_t)_mm256_movemask_ps(mask2) << 6;
        const __m256i *mp = masklut + (m >> 5);
        __m256i wa = _mm256_load_si256(mp);
        __m256i wb = _mm256_load_si256(mp + 1);
        xi = _mm256_permutevar8x32_ps(xi, wa);
        _mm256_maskstore_ps(z, wb, xi);
        z += _mm_popcnt_u32((unsigned)m);
        mp = masklut + (m2 >> 5);
        wa = _mm256_load_si256(mp);
        wb = _mm256_load_si256(mp + 1);
        xi2 = _mm256_permutevar8x32_ps(xi2, wa);
        _mm256_maskstore_ps(z, wb, xi2);
        z += _mm_popcnt_u32((unsigned)m2);
    }
    for (; x < xend; x++) {
        __m128 xi = _mm_load_ss(x);
        __m128 mask = _mm_cmpgt_ss(xi, _mm_set_ss(y));
        int m = _mm_extract_ps(mask, 0);
        if (m) {
            _mm_store_ss(z++, xi);
        }
    }
    return z - zbeg;
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
