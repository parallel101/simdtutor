#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include <algorithm>
#include <random>
#include <atomic>
#include <cstddef>
#include <x86intrin.h>
#include <cblas.h>

  /* vmovups  ymm0, YMMWORD PTR [rsi] */
  /* vfmadd213ps  ymm0, ymm1, YMMWORD PTR [rax] */
  /* add  rsi, 64 */
  /* add  rax, 64 */
  /* vmovups  YMMWORD PTR -64[rax], ymm0 */
  /* vmovups  ymm0, YMMWORD PTR -32[rsi] */
  /* vfmadd213ps  ymm0, ymm1, YMMWORD PTR -32[rax] */
  /* vmovups  YMMWORD PTR -32[rax], ymm0 */
  /* cmp  r9, rsi */
  /* jne  .L4 */

  /* vmovups  ymm2, YMMWORD PTR [rsi] */
  /* vmovups  ymm0, YMMWORD PTR 32[rsi] */
  /* add  rsi, 64 */
  /* add  rax, 64 */
  /* vfmadd213ps  ymm2, ymm1, YMMWORD PTR -64[rax] */
  /* vfmadd213ps  ymm0, ymm1, YMMWORD PTR -32[rax] */
  /* vmovups  YMMWORD PTR -64[rax], ymm2 */
  /* vmovups  YMMWORD PTR -32[rax], ymm0 */
  /* cmp  r9, rsi */
  /* jne  .L4 */

// BEGIN CODE
void simd_saxpy(int n, float alpha, float const *X, int incX, float *__restrict Y, int incY) {
    __m256 A = _mm256_set1_ps(alpha);
    float const *Xe = X + (n & ~15) * incX;
    if (incX == 1 && incY == 1) {
        while (X != Xe) {
            __m256 X1 = _mm256_loadu_ps(X);
            __m256 X2 = _mm256_loadu_ps(X + 8);
            __m256 Y1 = _mm256_loadu_ps(Y);
            __m256 Y2 = _mm256_loadu_ps(Y + 8);
            __m256 R1 = _mm256_fmadd_ps(A, X1, Y1);
            __m256 R2 = _mm256_fmadd_ps(A, X2, Y2);
            _mm256_storeu_ps(Y, R1);
            _mm256_storeu_ps(Y + 8, R2);
            X += 16;
            Y += 16;
        }
    } else if (incY == 1) {
        const __m256i Inc = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);
        __m256i IncX = _mm256_mullo_epi32(Inc, _mm256_set1_epi32(incX));
        while (X != Xe) {
            __m256 X1 = _mm256_i32gather_ps(X, IncX, sizeof(float));
            __m256 Y1 = _mm256_loadu_ps(Y);
            __m256 X2 = _mm256_i32gather_ps(X + 8 * incX, IncX, sizeof(float));
            __m256 Y2 = _mm256_loadu_ps(Y + 8);
            __m256 R1 = _mm256_fmadd_ps(A, X1, Y1);
            __m256 R2 = _mm256_fmadd_ps(A, X2, Y2);
            _mm256_storeu_ps(Y, R1);
            _mm256_storeu_ps(Y + 8, R2);
            X += 16 * incX;
            Y += 16;
        }
    } else {
        const __m256i Inc = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);
        __m256i IncX = _mm256_mullo_epi32(Inc, _mm256_set1_epi32(incX));
        __m256i IncY = _mm256_mullo_epi32(Inc, _mm256_set1_epi32(incY));
        while (X != Xe) {
            __m256 X1 = _mm256_i32gather_ps(X, IncX, sizeof(float));
            __m256 Y1 = _mm256_i32gather_ps(Y, IncY, sizeof(float));
            __m256 R1 = _mm256_fmadd_ps(A, X1, Y1);
            __m128 L = _mm256_castps256_ps128(R1);
            __m128 H = _mm256_extractf128_ps(R1, 1);
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
        *Y += alpha * *X;
        X += incX;
        Y += incY;
        std::atomic_signal_fence(std::memory_order_acq_rel);
    }
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
        simd_saxpy(n, a, x.data(), 1, y.data(), 1);
        benchmark::DoNotOptimize(x);
        benchmark::DoNotOptimize(y);
    }
    s.SetItemsProcessed(n * s.iterations());
}
BENCHMARK(bench);

TEST(MySuite, MyTest) {
    const auto n = size_t(8193);
    auto a = 0.5f;
    auto x = std::vector<float>(n * 3);
    auto y0 = std::vector<float>(n * 2);
    auto y1 = std::vector<float>();
    auto y2 = std::vector<float>();
    std::generate(x.begin(), x.end(), [uni = std::uniform_real_distribution<float>(), rng = std::mt19937()] () mutable { return uni(rng); });
    std::generate(y0.begin(), y0.end(), [uni = std::uniform_real_distribution<float>(), rng = std::mt19937()] () mutable { return uni(rng); });
    y2 = y1 = y0;
    simd_saxpy(n, a, x.data(), 3, y1.data(), 2);
    cblas_saxpy(n, a, x.data(), 3, y2.data(), 2);
    for (size_t i = 0; i < n * 2; i++) {
        EXPECT_EQ(y1[i], y2[i]);
        if (y1[i] != y2[i]) break;
    }
}
