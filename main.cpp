#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include <random>
#include <memory>
#include <cstddef>
#include <immintrin.h>

// BEGIN CODE
[[gnu::optimize("O2")]] size_t countp(float const *x, size_t n, float y) {
    int ret = 0;
    __m128 yv = _mm_set1_ps(y);
    for (size_t i = 0; i < n; i += 4) {
        __m128 xi = _mm_loadu_ps(&x[i]);
        __m128 mask = _mm_cmpgt_ps(xi, yv);
        int m = _mm_movemask_ps(mask);
        ret += _mm_popcnt_u32(m);
    }
    return ret;
}
// END CODE

static void bench(benchmark::State &s) {
    const auto n = size_t(8192);
    auto x = std::vector<float>(n);
    auto y = 0.5f;
    std::generate(x.begin(), x.end(), [uni = std::uniform_real_distribution<float>(), rng = std::mt19937()] () mutable { return uni(rng); });
    for (auto _: s) {
        auto ret = countp(x.data(), n, y);
        benchmark::DoNotOptimize(x);
        benchmark::DoNotOptimize(ret);
    }
    s.SetItemsProcessed(n * s.iterations());
}
BENCHMARK(bench);

size_t scalar_countp(float const *x, size_t n, float y) {
    size_t ret = 0;
    for (size_t i = 0; i < n; i++) {
        ret += x[i] > y ? 1 : 0;
    }
    return ret;
}

TEST(MySuite, MyTest) {
    const auto n = size_t(16384);
    auto x = std::vector<float>(n);
    auto y = 0.5f;
    std::generate(x.begin(), x.end(), [uni = std::uniform_real_distribution<float>(), rng = std::mt19937()] () mutable { return uni(rng); });
    auto ret = countp(x.data(), n, y);
    auto ret2 = scalar_countp(x.data(), n, y);
    EXPECT_NEAR(ret, ret2, 0.05f);
}
