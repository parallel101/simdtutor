#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include <random>
#include <memory>
#include <cstddef>
#include <immintrin.h>

// BEGIN CODE
float sum(float const *x, size_t n) {
    __m128 ret = _mm_setzero_ps();
    __m128 ret2 = _mm_setzero_ps();
    size_t i;
    for (i = 0; i + 8 <= n; i += 8) {
        ret = _mm_add_ps(ret, _mm_loadu_ps(x + i));
        ret2 = _mm_add_ps(ret2, _mm_loadu_ps(x + i + 4));
    }
    ret = _mm_add_ps(ret, ret2);
    ret = _mm_hadd_ps(ret, ret);
    ret = _mm_hadd_ps(ret, ret);
    for (; i < n; i++) {
        ret = _mm_add_ss(ret, _mm_load_ss(x + i));
    }
    return _mm_cvtss_f32(ret);
}
// END CODE

static void bench(benchmark::State &s) {
    const auto n = size_t(8192);
    auto x = std::vector<float>(n);
    std::generate(x.begin(), x.end(), [uni = std::uniform_real_distribution<float>(), rng = std::mt19937()] () mutable { return uni(rng); });
    for (auto _: s) {
        auto ret = sum(x.data(), n);
        benchmark::DoNotOptimize(x);
        benchmark::DoNotOptimize(ret);
    }
    s.SetItemsProcessed(n * s.iterations());
}
BENCHMARK(bench);

float scalar_sum(float const *x, size_t n) {
    float ret = 0.0f;
    for (size_t i = 0; i < n; i++) {
        ret += x[i];
    }
    return ret;
}

TEST(MySuite, MyTest) {
    const auto n = size_t(16384);
    auto x = std::vector<float>(n);
    std::generate(x.begin(), x.end(), [uni = std::uniform_real_distribution<float>(), rng = std::mt19937()] () mutable { return uni(rng); });
    auto ret = sum(x.data(), n);
    auto ret2 = scalar_sum(x.data(), n);
    EXPECT_NEAR(ret, ret2, 0.05f);
}
