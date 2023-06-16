#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include <random>
#include <memory>
#include <cstddef>
#include <immintrin.h>

// BEGIN CODE
size_t countp(float const *x, size_t n, float y) {
    __m128 pred = _mm_set1_ps(y);
    __m128i ret = _mm_setzero_si128();
    size_t i;
    for (i = 0; i + 8 <= n; i += 8) {
        __m128 xi = _mm_loadu_ps(x + i);
        __m128i mask = _mm_castps_si128(_mm_cmpgt_ps(xi, pred));
        __m128 xi2 = _mm_loadu_ps(x + i + 4);
        __m128i mask2 = _mm_castps_si128(_mm_cmpgt_ps(xi2, pred));
        ret = _mm_sub_epi32(ret, _mm_add_epi32(mask, mask2));
    }
    for (; i < n; i++) {
        __m128 xi = _mm_load_ss(x + i);
        __m128 mask = _mm_cmpgt_ss(xi, pred);
        int m = _mm_extract_ps(mask, 0);
        ret += !!m;
    }
    ret = _mm_add_epi32(ret, _mm_shuffle_epi32(ret, 0b01001110));
    return _mm_extract_epi32(ret, 0) + _mm_extract_epi32(ret, 1);
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
