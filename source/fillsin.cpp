#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include <random>
#include <memory>
#include <cstddef>
#include <immintrin.h>
#include <cmath>

static __m256 mm256_sincos_ps(__m256 xx, __m256 *cosret) {
    const __m256 DP1F = _mm256_set1_ps(0.78515625f * 2.f);
    const __m256 DP2F = _mm256_set1_ps(2.4187564849853515625E-4f * 2.f);
    const __m256 DP3F = _mm256_set1_ps(3.77489497744594108E-8f * 2.f);
    const __m256 P0sinf = _mm256_set1_ps(-1.6666654611E-1f);
    const __m256 P1sinf = _mm256_set1_ps(8.3321608736E-3f);
    const __m256 P2sinf = _mm256_set1_ps(-1.9515295891E-4f);
    const __m256 P0cosf = _mm256_set1_ps(4.166664568298827E-2f);
    const __m256 P1cosf = _mm256_set1_ps(-1.388731625493765E-3f);
    const __m256 P2cosf = _mm256_set1_ps(2.443315711809948E-5f);

    __m256 xa = _mm256_and_ps(xx, _mm256_castsi256_ps(_mm256_set1_epi32(0x7FFFFFFF)));
    __m256i q = _mm256_cvtps_epi32(_mm256_mul_ps(xa, _mm256_set1_ps(2.f / M_PI)));
    __m256 y = _mm256_cvtepi32_ps(q);
    __m256 x = _mm256_fnmadd_ps(y, DP3F, _mm256_fnmadd_ps(y, DP2F, _mm256_fnmadd_ps(y, DP1F, xa)));
    __m256 x2 = _mm256_mul_ps(x, x);
    __m256 x3 = _mm256_mul_ps(x2, x);
    __m256 x4 = _mm256_mul_ps(x2, x2);
    __m256 s = _mm256_fmadd_ps(x3, _mm256_fmadd_ps(x2, P2sinf, _mm256_fmadd_ps(x, P1sinf, P0sinf)), x);
    __m256 c = _mm256_fmadd_ps(x4, _mm256_fmadd_ps(x2, P2cosf, _mm256_fmadd_ps(x, P1cosf, P0cosf)), _mm256_fnmadd_ps(_mm256_set1_ps(0.5f), x2, _mm256_set1_ps(1.0f)));
    __m256 mask = _mm256_castsi256_ps(_mm256_cmpeq_epi32(_mm256_setzero_si256(), _mm256_castps_si256(_mm256_and_ps(_mm256_castsi256_ps(q), _mm256_castsi256_ps(_mm256_set1_epi32(0x1))))));
    __m256 sin1 = _mm256_blendv_ps(c, s, mask);
    __m256 cos1 = _mm256_blendv_ps(s, c, mask);
    /* __m256 sin1 = _mm256_or_ps(_mm256_and_ps(c, swap), _mm256_andnot_ps(s, swap)); */
    /* __m256 cos1 = _mm256_or_ps(_mm256_and_ps(s, swap), _mm256_andnot_ps(c, swap)); */
    sin1 = _mm256_xor_ps(sin1, _mm256_and_ps(_mm256_xor_ps(_mm256_castsi256_ps(_mm256_slli_epi32(q, 30)), xx), _mm256_set1_ps(-0.0f)));
    cos1 = _mm256_xor_ps(cos1, _mm256_castsi256_ps(_mm256_slli_epi32(_mm256_castps_si256(_mm256_and_ps(_mm256_castsi256_ps(_mm256_add_epi32(q, _mm256_set1_epi32(1))), _mm256_castsi256_ps(_mm256_set1_epi32(0x2)))), 30)));
    *cosret = cos1;
    return sin1;
}

// BEGIN CODE
void fillsin(float *x, size_t n) {
    __m256i idx = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);
    for (size_t i = 0; i < n; i += 8) {
        __m256 ii = _mm256_cvtepi32_ps(idx);
        __m256 cosi;
        __m256 sini = mm256_sincos_ps(ii, &cosi);
        _mm256_store_ps(&x[i], sini);
        idx = _mm256_add_epi32(idx, _mm256_set1_epi32(8));
    }
}
// END CODE

static void bench(benchmark::State &s) {
    const auto n = size_t(8192);
    auto x = std::vector<float>(n);
    auto y = 0.99993f;
    std::generate(x.begin(), x.end(), [uni = std::uniform_real_distribution<float>(), rng = std::mt19937()] () mutable { return uni(rng); });
    for (auto _: s) {
        fillsin(x.data(), n);
        benchmark::DoNotOptimize(x);
    }
    s.SetItemsProcessed(n * s.iterations());
}
BENCHMARK(bench);

void scalar_fillsin(float *x, size_t n) {
    for (size_t i = 0; i < n; i++) {
        x[i] = sinf(i);
    }
}


TEST(MySuite, MyTest) {
    const auto n = size_t(16384);
    auto x = std::vector<float>(n);
    auto x2 = std::vector<float>(n);
    fillsin(x.data(), n);
    scalar_fillsin(x2.data(), n);
    for (size_t i = 0; i < n; i++) {
        EXPECT_NEAR(x[i], x2[i], 0.01f);
    }
}
