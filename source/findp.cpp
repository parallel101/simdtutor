#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include <random>
#include <memory>
#include <cstddef>
#include <immintrin.h>

// BEGIN CODE
size_t findp(float const *x, size_t n, float y) {
    __m128 pred = _mm_set1_ps(y);
    __m128i index = _mm_setr_epi32(0, 1, 2, 3);
    size_t i;
    for (i = 0; i + 16 <= n; i += 16) {
        __m128 xi = _mm_loadu_ps(x + i);
        __m128 xi2 = _mm_loadu_ps(x + i + 4);
        __m128 xi3 = _mm_loadu_ps(x + i + 8);
        __m128 xi4 = _mm_loadu_ps(x + i + 12);
        xi = _mm_cmpgt_ps(xi, pred);
        xi2 = _mm_cmpgt_ps(xi2, pred);
        xi3 = _mm_cmpgt_ps(xi3, pred);
        xi4 = _mm_cmpgt_ps(xi4, pred);
        xi = _mm_or_ps(xi, xi2);
        xi3 = _mm_or_ps(xi3, xi4);
        xi = _mm_or_ps(xi, xi3);
        if (_mm_movemask_ps(xi)) [[unlikely]] {
            __m128 xi = _mm_loadu_ps(x + i);
            __m128 xi2 = _mm_loadu_ps(x + i + 4);
            __m128 xi3 = _mm_loadu_ps(x + i + 8);
            __m128 xi4 = _mm_loadu_ps(x + i + 12);
            xi = _mm_cmpgt_ps(xi, pred);
            xi2 = _mm_cmpgt_ps(xi2, pred);
            xi3 = _mm_cmpgt_ps(xi3, pred);
            xi4 = _mm_cmpgt_ps(xi4, pred);
            int m = _mm_movemask_ps(xi);
            int m2 = _mm_movemask_ps(xi2);
            int m3 = _mm_movemask_ps(xi3);
            int m4 = _mm_movemask_ps(xi4);
            m |= m2 << 4;
            m3 |= m4 << 4;
            m |= m3 << 8;
            return _tzcnt_u32(m) + i;
        }
    }
    for (; i < n; i++) {
        __m128 xi = _mm_load_ss(x + i);
        __m128 mask = _mm_cmpgt_ss(xi, pred);
        int m = _mm_extract_ps(mask, 0);
        if (m) {
            return i;
        }
    }
    return (size_t)-1;
}
// END CODE

static void bench(benchmark::State &s) {
    const auto n = size_t(8192);
    auto x = std::vector<float>(n);
    auto y = 0.99993f;
    std::generate(x.begin(), x.end(), [uni = std::uniform_real_distribution<float>(), rng = std::mt19937()] () mutable { return uni(rng); });
    for (auto _: s) {
        auto ret = findp(x.data(), n, y);
        benchmark::DoNotOptimize(x);
        benchmark::DoNotOptimize(ret);
    }
    s.SetItemsProcessed(n * s.iterations());
}
BENCHMARK(bench);

size_t scalar_findp(float const *x, size_t n, float y) {
    for (size_t i = 0; i < n; i++) {
        if (x[i] > y)
            return i;
    }
    return (size_t)-1;
}

TEST(MySuite, MyTest) {
    const auto n = size_t(16384);
    auto x = std::vector<float>(n);
    auto y = 0.99993f;
    std::generate(x.begin(), x.end(), [uni = std::uniform_real_distribution<float>(), rng = std::mt19937()] () mutable { return uni(rng); });
    auto ret = findp(x.data(), n, y);
    auto ret2 = scalar_findp(x.data(), n, y);
    EXPECT_NEAR(ret, ret2, 0.05f);
}
