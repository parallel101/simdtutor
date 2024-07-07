#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include <algorithm>
#include <random>
#include <atomic>
#include <cstddef>
#include <x86intrin.h>
#include <gsl/gsl_cblas.h>

// BEGIN CODE
float simd_sdot(int n, float const *X, int incX, float const *Y, int incY) {
    __m256 Sum1 = _mm256_setzero_ps();
    __m256 Sum2 = _mm256_setzero_ps();
    float const *Xe = X + (n & ~15) * incX;
    if (incX == 1 && incY == 1) {
        while (X != Xe) {
            __m256 X1 = _mm256_loadu_ps(X);
            __m256 X2 = _mm256_loadu_ps(X + 8);
            __m256 Y1 = _mm256_loadu_ps(Y);
            __m256 Y2 = _mm256_loadu_ps(Y + 8);
            Sum1 = _mm256_fmadd_ps(X1, Y1, Sum1);
            Sum2 = _mm256_fmadd_ps(X2, Y2, Sum2);
            X += 16;
            Y += 16;
        }
    } else {
        const __m256i Inc = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);
        __m256i IncX = _mm256_mullo_epi32(Inc, _mm256_set1_epi32(incX));
        __m256i IncY = _mm256_mullo_epi32(Inc, _mm256_set1_epi32(incY));
        while (X != Xe) {
            __m256 X1 = _mm256_i32gather_ps(X, IncX, sizeof(float));
            __m256 X2 = _mm256_i32gather_ps(X + 8 * incX, IncX, sizeof(float));
            __m256 Y1 = _mm256_i32gather_ps(Y, IncY, sizeof(float));
            __m256 Y2 = _mm256_i32gather_ps(Y + 8 * incY, IncY, sizeof(float));
            Sum1 = _mm256_fmadd_ps(X1, Y1, Sum1);
            Sum2 = _mm256_fmadd_ps(X2, Y2, Sum2);
            X += 16 * incX;
            Y += 16 * incY;
        }
    }
    Sum1 = _mm256_add_ps(Sum1, Sum2);
    Sum1 = _mm256_hadd_ps(Sum1, Sum1);
    Sum1 = _mm256_hadd_ps(Sum1, Sum1);
    Sum1 = _mm256_hadd_ps(Sum1, Sum1);
    float sum = _mm256_cvtss_f32(Sum1);
    Xe = X + (n & 15) * incX;
    while (X != Xe) {
        sum += *X * *Y;
        X += incX;
        Y += incY;
        std::atomic_signal_fence(std::memory_order_acq_rel);
    }
    return sum;
}
// END CODE

static void bench(benchmark::State &s) {
    const auto n = size_t(8192);
    auto a = 0.5f;
    auto x = std::vector<float>(n * 2);
    auto y = std::vector<float>(n);
    std::generate(x.begin(), x.end(), [uni = std::uniform_real_distribution<float>(), rng = std::mt19937()] () mutable { return uni(rng); });
    std::generate(y.begin(), y.end(), [uni = std::uniform_real_distribution<float>(), rng = std::mt19937()] () mutable { return uni(rng); });
    for (auto _: s) {
        float res = simd_sdot(n, x.data(), 2, y.data(), 1);
        benchmark::DoNotOptimize(res);
        benchmark::DoNotOptimize(x);
        benchmark::DoNotOptimize(y);
    }
    s.SetItemsProcessed(n * s.iterations());
}
BENCHMARK(bench);

TEST(MySuite, MyTest) {
    const auto n = size_t(8193);
    auto x = std::vector<float>(n * 2);
    auto y = std::vector<float>(n);
    std::generate(x.begin(), x.end(), [uni = std::uniform_real_distribution<float>(), rng = std::mt19937()] () mutable { return uni(rng); });
    std::generate(y.begin(), y.end(), [uni = std::uniform_real_distribution<float>(), rng = std::mt19937()] () mutable { return uni(rng); });
    float res1 = simd_sdot(n, x.data(), 2, y.data(), 1);
    float res2 = cblas_sdot(n, x.data(), 2, y.data(), 1);
    EXPECT_NEAR(res1, res2, n * 0.003f);
}
