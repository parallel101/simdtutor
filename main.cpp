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
    for (size_t i = 0; i < n; i += 16) {
        __m128 xi = _mm_loadu_ps(&x[i]);
        __m128 xi2 = _mm_loadu_ps(&x[i + 4]);
        __m128 xi3 = _mm_loadu_ps(&x[i + 8]);
        __m128 xi4 = _mm_loadu_ps(&x[i + 12]);
        xi = _mm_add_ps(xi, xi3);
        xi2 = _mm_add_ps(xi2, xi4);
        ret = _mm_add_ps(ret, xi);
        ret2 = _mm_add_ps(ret2, xi2);
    }
    ret = _mm_add_ps(ret, ret2);
    ret = _mm_add_ss(ret, _mm_shuffle_ps(ret, ret, _MM_SHUFFLE(0, 0, 0, 1)));
    ret = _mm_add_ss(ret, _mm_shuffle_ps(ret, ret, _MM_SHUFFLE(0, 0, 0, 2)));
    ret = _mm_add_ss(ret, _mm_shuffle_ps(ret, ret, _MM_SHUFFLE(0, 0, 0, 3)));
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
