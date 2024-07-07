#include <benchmark/benchmark.h>
#include <cmath>
#include <gtest/gtest.h>
#include <algorithm>
#include <random>
#include <atomic>
#include <cstddef>
#include <x86intrin.h>
#include <cblas.h>

// BEGIN CODE
void simd_srot(int n, float *__restrict X, int incX, float *__restrict Y, int incY, const float c, const float s) {
    __m256 C = _mm256_set1_ps(c);
    __m256 S = _mm256_set1_ps(s);
    float const *Xe = X + (n & ~15) * incX;
    if (incX == 1 && incY == 1) {
        while (X != Xe) {
            __m256 X1 = _mm256_loadu_ps(X);
            __m256 X2 = _mm256_loadu_ps(X + 8);
            __m256 Y1 = _mm256_loadu_ps(Y);
            __m256 Y2 = _mm256_loadu_ps(Y + 8);
            __m256 NX1 = _mm256_fmadd_ps(C, X1, _mm256_mul_ps(S, Y1));
            __m256 NX2 = _mm256_fmadd_ps(C, X2, _mm256_mul_ps(S, Y2));
            __m256 NY1 = _mm256_fnmadd_ps(S, X1, _mm256_mul_ps(C, Y1));
            __m256 NY2 = _mm256_fnmadd_ps(S, X2, _mm256_mul_ps(C, Y2));
            _mm256_storeu_ps(X, NX1);
            _mm256_storeu_ps(X + 8, NX2);
            _mm256_storeu_ps(Y, NY1);
            _mm256_storeu_ps(Y + 8, NY2);
            X += 16;
            Y += 16;
        }
    } else {
        const __m256i Inc = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);
        __m256i IncX = _mm256_mullo_epi32(Inc, _mm256_set1_epi32(incX));
        __m256i IncY = _mm256_mullo_epi32(Inc, _mm256_set1_epi32(incY));
        while (X != Xe) {
            __m256 X1 = _mm256_i32gather_ps(X, IncX, sizeof(float));
            __m256 Y1 = _mm256_i32gather_ps(Y, IncY, sizeof(float));
            __m256 NX1 = _mm256_fmadd_ps(C, X1, _mm256_mul_ps(S, Y1));
            __m256 NY1 = _mm256_fnmadd_ps(S, X1, _mm256_mul_ps(C, Y1));
            __m128 L = _mm256_castps256_ps128(NX1);
            __m128 H = _mm256_extractf128_ps(NX1, 1);
            _MM_EXTRACT_FLOAT(*X, L, 0);
            _MM_EXTRACT_FLOAT(X[incX], L, 1);
            _MM_EXTRACT_FLOAT(X[2 * incX], L, 2);
            _MM_EXTRACT_FLOAT(X[3 * incX], L, 3);
            _MM_EXTRACT_FLOAT(X[4 * incX], H, 0);
            _MM_EXTRACT_FLOAT(X[5 * incX], H, 1);
            _MM_EXTRACT_FLOAT(X[6 * incX], H, 2);
            _MM_EXTRACT_FLOAT(X[7 * incX], H, 3);
            L = _mm256_castps256_ps128(NY1);
            H = _mm256_extractf128_ps(NY1, 1);
            _MM_EXTRACT_FLOAT(*Y, L, 0);
            _MM_EXTRACT_FLOAT(Y[incY], L, 1);
            _MM_EXTRACT_FLOAT(Y[2 * incY], L, 2);
            _MM_EXTRACT_FLOAT(Y[3 * incY], L, 3);
            _MM_EXTRACT_FLOAT(Y[4 * incY], H, 0);
            _MM_EXTRACT_FLOAT(Y[5 * incY], H, 1);
            _MM_EXTRACT_FLOAT(Y[6 * incY], H, 2);
            _MM_EXTRACT_FLOAT(Y[7 * incY], H, 3);
            X += 8 * incX;
            Y += 8 * incY;
        }
    }
    Xe = X + (n & 15) * incX;
    while (X != Xe) {
        float xi = c * *X + s * *Y;
        *Y = c * *Y - s * *X;
        *X = xi;
        X += incX;
        Y += incY;
        std::atomic_signal_fence(std::memory_order_acq_rel);
    }
}
// END CODE

static void bench(benchmark::State &s) {
    const auto n = size_t(8192);
    auto cos = sqrtf(2);
    auto sin = sqrtf(2);
    auto x = std::vector<float>(n);
    auto y = std::vector<float>(n);
    std::generate(x.begin(), x.end(), [uni = std::uniform_real_distribution<float>(), rng = std::mt19937()] () mutable { return uni(rng); });
    std::generate(y.begin(), y.end(), [uni = std::uniform_real_distribution<float>(), rng = std::mt19937()] () mutable { return uni(rng); });
    for (auto _: s) {
        simd_srot(n, x.data(), 1, y.data(), 1, cos, sin);
        benchmark::DoNotOptimize(x);
        benchmark::DoNotOptimize(y);
    }
    s.SetItemsProcessed(n * s.iterations());
}
BENCHMARK(bench);

TEST(MySuite, MyTest) {
    const auto n = size_t(8193);
    auto cos = 0.51f;
    auto sin = 0.73f;
    auto x0 = std::vector<float>(n * 3);
    auto x1 = std::vector<float>();
    auto x2 = std::vector<float>();
    auto y0 = std::vector<float>(n * 2);
    auto y1 = std::vector<float>();
    auto y2 = std::vector<float>();
    std::generate(x0.begin(), x0.end(), [uni = std::uniform_real_distribution<float>(), rng = std::mt19937()] () mutable { return uni(rng); });
    std::generate(y0.begin(), y0.end(), [uni = std::uniform_real_distribution<float>(), rng = std::mt19937()] () mutable { return uni(rng); });
    y2 = y1 = y0;
    x2 = x1 = x0;
    simd_srot(n, x1.data(), 3, y1.data(), 2, cos, sin);
    cblas_srot(n, x2.data(), 3, y2.data(), 2, cos, sin);
    for (size_t i = 0; i < n * 2; i++) {
        EXPECT_FLOAT_EQ(y1[i], y2[i]);
        if (y1[i] != y2[i]) break;
        EXPECT_FLOAT_EQ(x1[i], x2[i]);
        if (x1[i] != x2[i]) break;
    }
}
