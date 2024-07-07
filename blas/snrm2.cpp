#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include <algorithm>
#include <random>
#include <cmath>
#include <atomic>
#include <cstddef>
#include <x86intrin.h>
#include <gsl/gsl_cblas.h>

// BEGIN CODE
float simd_snrm2(int n, float const *X, int incX) {
    __m256 Sum1 = _mm256_setzero_ps();
    __m256 Sum2 = _mm256_setzero_ps();
    float const *Xe = X + (n & ~31) * incX;
    if (incX == 1) {
        while (X != Xe) {
            __m256 X1 = _mm256_loadu_ps(X);
            __m256 X2 = _mm256_loadu_ps(X + 8);
            __m256 X3 = _mm256_loadu_ps(X + 16);
            __m256 X4 = _mm256_loadu_ps(X + 24);
            Sum1 = _mm256_fmadd_ps(X1, X1, Sum1);
            Sum2 = _mm256_fmadd_ps(X2, X2, Sum2);
            Sum1 = _mm256_fmadd_ps(X3, X3, Sum1);
            Sum2 = _mm256_fmadd_ps(X4, X4, Sum2);
            X += 32;
        }
    } else {
        const __m256i Inc = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);
        __m256i IncX = _mm256_mullo_epi32(Inc, _mm256_set1_epi32(incX));
        while (X != Xe) {
            __m256 X1 = _mm256_i32gather_ps(X, IncX, sizeof(float));
            X += 8 * incX;
            __m256 X2 = _mm256_i32gather_ps(X, IncX, sizeof(float));
            Sum1 = _mm256_fmadd_ps(X1, X1, Sum1);
            Sum2 = _mm256_fmadd_ps(X2, X2, Sum2);
            X += 8 * incX;
            __m256 X3 = _mm256_i32gather_ps(X, IncX, sizeof(float));
            X += 8 * incX;
            __m256 X4 = _mm256_i32gather_ps(X, IncX, sizeof(float));
            Sum1 = _mm256_fmadd_ps(X3, X3, Sum1);
            Sum2 = _mm256_fmadd_ps(X3, X4, Sum2);
            X += 8 * incX;
        }
    }
    Sum1 = _mm256_add_ps(Sum1, Sum2);
    Sum1 = _mm256_hadd_ps(Sum1, Sum1);
    Sum1 = _mm256_hadd_ps(Sum1, Sum1);
    Sum1 = _mm256_hadd_ps(Sum1, Sum1);
    float sum = _mm256_cvtss_f32(Sum1);
    Xe = X + (n & 31) * incX;
    while (X != Xe) {
        sum += *X * *X;
        X += incX;
        std::atomic_signal_fence(std::memory_order_acq_rel);
    }
    return sqrtf(sum);
}
// END CODE

static void bench(benchmark::State &s) {
    const auto n = size_t(8192);
    auto a = 0.5f;
    auto x = std::vector<float>(n);
    std::generate(x.begin(), x.end(), [uni = std::uniform_real_distribution<float>(), rng = std::mt19937()] () mutable { return uni(rng); });
    for (auto _: s) {
        float res = simd_snrm2(n, x.data(), 1);
        benchmark::DoNotOptimize(res);
        benchmark::DoNotOptimize(x);
    }
    s.SetItemsProcessed(n * s.iterations());
}
BENCHMARK(bench);

TEST(MySuite, MyTest) {
    const auto n = size_t(8193);
    auto x = std::vector<float>(n);
    std::generate(x.begin(), x.end(), [uni = std::uniform_real_distribution<float>(), rng = std::mt19937()] () mutable { return uni(rng); });
    float res1 = simd_snrm2(n, x.data(), 1);
    float res2 = cblas_snrm2(n, x.data(), 1);
    EXPECT_NEAR(res1, res2, n * 0.002f);
}
