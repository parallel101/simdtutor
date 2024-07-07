#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include <algorithm>
#include <random>
#include <atomic>
#include <cstddef>
#include <x86intrin.h>
#include <gsl/gsl_cblas.h>

// BEGIN CODE
void simd_scopy(int n, float const *X, int incX, float *__restrict Y, int incY) {
    float const *Xe = X + (n & ~31) * incX;
    if (incX == 1 && incY == 1) {
        if (n >= 65536 && ((uintptr_t)Y & 15) == 0 && ((uintptr_t)X & 15) == 0) {
            if (((uintptr_t)Y & 31) == 0 && ((uintptr_t)X & 31) == 0) {
                while (X != Xe) {
                    __m256i const *Xi = (__m256i const *)X;
                    __m256i *Yi = (__m256i *)Y;
                    __m256i X1 = _mm256_stream_load_si256(Xi);
                    __m256i X2 = _mm256_stream_load_si256(Xi + 1);
                    _mm256_stream_si256(Yi, X1);
                    _mm256_stream_si256(Yi + 1, X2);
                    __m256i X3 = _mm256_stream_load_si256(Xi + 2);
                    __m256i X4 = _mm256_stream_load_si256(Xi + 3);
                    _mm256_stream_si256(Yi + 2, X3);
                    _mm256_stream_si256(Yi + 3, X4);
                    X += 32;
                    Y += 32;
                }
            } else {
                while (X != Xe) {
                    __m128i *Xi = (__m128i *)X;
                    __m128i *Yi = (__m128i *)Y;
                    __m128i X1 = _mm_stream_load_si128(Xi);
                    __m128i X2 = _mm_stream_load_si128(Xi + 1);
                    _mm_stream_si128(Yi, X1);
                    _mm_stream_si128(Yi + 1, X2);
                    __m128i X3 = _mm_stream_load_si128(Xi + 2);
                    __m128i X4 = _mm_stream_load_si128(Xi + 3);
                    _mm_stream_si128(Yi + 2, X3);
                    _mm_stream_si128(Yi + 3, X4);
                    X += 16;
                    Y += 16;
                }
            }
        } else {
            while (X != Xe) {
                __m256 X1 = _mm256_loadu_ps(X);
                __m256 X2 = _mm256_loadu_ps(X + 8);
                _mm256_storeu_ps(Y, X1);
                _mm256_storeu_ps(Y + 8, X2);
                __m256 X3 = _mm256_loadu_ps(X + 16);
                __m256 X4 = _mm256_loadu_ps(X + 24);
                _mm256_storeu_ps(Y + 16, X3);
                _mm256_storeu_ps(Y + 24, X4);
                X += 32;
                Y += 32;
            }
        }
    } else if (incY == 1) {
        const __m256i Inc = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);
        __m256i IncX = _mm256_mullo_epi32(Inc, _mm256_set1_epi32(incX));
        while (X != Xe) {
            __m256 X1 = _mm256_i32gather_ps(X, IncX, sizeof(float));
            __m256 X2 = _mm256_i32gather_ps(X + 8 * incX, IncX, sizeof(float));
            _mm256_storeu_ps(Y, X1);
            _mm256_storeu_ps(Y + 8, X2);
            X += 16 * incX;
            Y += 16;
        }
    } else {
        const __m256i Inc = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);
        __m256i IncX = _mm256_mullo_epi32(Inc, _mm256_set1_epi32(incX));
        while (X != Xe) {
            __m256 X1 = _mm256_i32gather_ps(X, IncX, sizeof(float));
            __m128 L = _mm256_castps256_ps128(X1);
            __m128 H = _mm256_extractf128_ps(X1, 1);
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
    Xe = X + (n & 31) * incX;
    while (X != Xe) {
        *Y = *X;
        X += incX;
        Y += incY;
        std::atomic_signal_fence(std::memory_order_acq_rel);
    }
}
// END CODE

static void bench(benchmark::State &s) {
    const auto n = size_t(8192);
    auto x = std::vector<float>(n);
    auto y = std::vector<float>(n);
    std::generate(x.begin(), x.end(), [uni = std::uniform_real_distribution<float>(), rng = std::mt19937()] () mutable { return uni(rng); });
    for (auto _: s) {
        simd_scopy(n, x.data(), 1, y.data(), 1);
        benchmark::DoNotOptimize(x);
        benchmark::DoNotOptimize(y);
    }
    s.SetItemsProcessed(n * s.iterations());
}
BENCHMARK(bench);

TEST(MySuite, MyTest) {
    const auto n = size_t(8193);
    auto x = std::vector<float>(n * 3);
    auto y1 = std::vector<float>(n * 2);
    auto y2 = std::vector<float>(n * 2);
    std::generate(x.begin(), x.end(), [uni = std::uniform_real_distribution<float>(), rng = std::mt19937()] () mutable { return uni(rng); });
    simd_scopy(n, x.data(), 3, y1.data(), 2);
    cblas_scopy(n, x.data(), 3, y2.data(), 2);
    for (size_t i = 0; i < n * 2; i++) {
        EXPECT_EQ(y1[i], y2[i]);
        if (y1[i] != y2[i]) break;
    }
}
