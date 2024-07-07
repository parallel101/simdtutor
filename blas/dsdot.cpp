#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include <algorithm>
#include <random>
#include <atomic>
#include <cstddef>
#include <x86intrin.h>
#include <cblas.h>

// BEGIN CODE
double simd_dsdot(int n, float const *X, int incX, float const *Y, int incY) {
    __m256d Sum1 = _mm256_setzero_pd();
    __m256d Sum2 = _mm256_setzero_pd();
    float const *Xe = X + (n & ~15) * incX;
    if (incX == 1 && incY == 1) {
        while (X != Xe) {
            __m256d X1 = _mm256_cvtps_pd(_mm_loadu_ps(X));
            __m256d X2 = _mm256_cvtps_pd(_mm_loadu_ps(X + 4));
            __m256d X3 = _mm256_cvtps_pd(_mm_loadu_ps(X + 8));
            __m256d X4 = _mm256_cvtps_pd(_mm_loadu_ps(X + 12));
            __m256d Y1 = _mm256_cvtps_pd(_mm_loadu_ps(Y));
            __m256d Y2 = _mm256_cvtps_pd(_mm_loadu_ps(Y + 4));
            __m256d Y3 = _mm256_cvtps_pd(_mm_loadu_ps(Y + 8));
            __m256d Y4 = _mm256_cvtps_pd(_mm_loadu_ps(Y + 12));
            Sum1 = _mm256_fmadd_pd(X1, Y1, Sum1);
            Sum2 = _mm256_fmadd_pd(X2, Y2, Sum2);
            Sum1 = _mm256_fmadd_pd(X3, Y3, Sum1);
            Sum2 = _mm256_fmadd_pd(X4, Y4, Sum2);
            X += 16;
            Y += 16;
        }
    } else {
        const __m128i Inc = _mm_setr_epi32(0, 1, 2, 3);
        __m128i IncX = _mm_mullo_epi32(Inc, _mm_set1_epi32(incX));
        __m128i IncY = _mm_mullo_epi32(Inc, _mm_set1_epi32(incY));
        while (X != Xe) {
            __m256d X1 = _mm256_cvtps_pd(_mm_i32gather_ps(X, IncX, sizeof(float)));
            __m256d X2 = _mm256_cvtps_pd(_mm_i32gather_ps(X + 4 * incX, IncX, sizeof(float)));
            X += 8 * incX;
            __m256d X3 = _mm256_cvtps_pd(_mm_i32gather_ps(X, IncX, sizeof(float)));
            __m256d X4 = _mm256_cvtps_pd(_mm_i32gather_ps(X + 4 * incX, IncX, sizeof(float)));
            X += 8 * incX;
            __m256d Y1 = _mm256_cvtps_pd(_mm_i32gather_ps(Y, IncY, sizeof(float)));
            __m256d Y2 = _mm256_cvtps_pd(_mm_i32gather_ps(Y + 4 * incY, IncY, sizeof(float)));
            Y += 8 * incY;
            __m256d Y3 = _mm256_cvtps_pd(_mm_i32gather_ps(Y, IncY, sizeof(float)));
            __m256d Y4 = _mm256_cvtps_pd(_mm_i32gather_ps(Y + 4 * incY, IncY, sizeof(float)));
            Y += 8 * incY;
            Sum1 = _mm256_fmadd_pd(X1, Y1, Sum1);
            Sum2 = _mm256_fmadd_pd(X2, Y2, Sum2);
            Sum1 = _mm256_fmadd_pd(X3, Y3, Sum1);
            Sum2 = _mm256_fmadd_pd(X4, Y4, Sum2);
        }
    }
    Sum1 = _mm256_add_pd(Sum1, Sum2);
    Sum1 = _mm256_hadd_pd(Sum1, Sum1);
    Sum1 = _mm256_hadd_pd(Sum1, Sum1);
    double sum = _mm256_cvtsd_f64(Sum1);
    Xe = X + (n & 15) * incX;
    while (X != Xe) {
        sum += (double)*X * (double)*Y;
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
    auto x = std::vector<float>(n);
    auto y = std::vector<float>(n);
    std::generate(x.begin(), x.end(), [uni = std::uniform_real_distribution<float>(), rng = std::mt19937()] () mutable { return uni(rng); });
    std::generate(y.begin(), y.end(), [uni = std::uniform_real_distribution<float>(), rng = std::mt19937()] () mutable { return uni(rng); });
    for (auto _: s) {
        double res = simd_dsdot(n, x.data(), 1, y.data(), 1);
        benchmark::DoNotOptimize(res);
        benchmark::DoNotOptimize(x);
        benchmark::DoNotOptimize(y);
    }
    s.SetItemsProcessed(n * s.iterations());
}
BENCHMARK(bench);

TEST(MySuite, MyTest) {
    const auto n = size_t(8193);
    auto x = std::vector<float>(n);
    auto y = std::vector<float>(n);
    std::generate(x.begin(), x.end(), [uni = std::uniform_real_distribution<float>(), rng = std::mt19937()] () mutable { return uni(rng); });
    std::generate(y.begin(), y.end(), [uni = std::uniform_real_distribution<float>(), rng = std::mt19937()] () mutable { return uni(rng); });
    double res1 = simd_dsdot(n, x.data(), 1, y.data(), 1);
    double res2 = cblas_dsdot(n, x.data(), 1, y.data(), 1);
    EXPECT_NEAR(res1, res2, n * 0.0015);
}
