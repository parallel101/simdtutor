#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include <random>
#include <memory>
#include <cstddef>
#include <immintrin.h>

// BEGIN CODE
void convolve(uint16_t const *__restrict in, uint16_t *__restrict out, uint16_t const *__restrict filter, size_t n, uint8_t shift) {
    for (size_t i = 0; i < n; i++) {
        uint16_t tmp = 0;
        for (size_t j = 0; j < 4; j++) {
            tmp += in[i + j] * filter[j];
        }
        out[i] = tmp >> shift;
    }
}
// END CODE
    /* auto v_filter = _mm_castpd_si128(_mm_loaddup_pd((double *)filter)); */

static void bench(benchmark::State &s) {
    const auto n = size_t(32768);
    auto in = std::vector<uint16_t>(n + 4);
    auto out = std::vector<uint16_t>(n);
    auto filter = std::vector<uint16_t>{1, 2, 3, 4};
    std::generate(in.begin(), in.end(), [uni = std::uniform_int_distribution<uint16_t>(0, 100), rng = std::mt19937()] () mutable { return uni(rng); });
    for (auto _: s) {
        convolve(in.data(), out.data(), filter.data(), n, 2);
        benchmark::DoNotOptimize(in);
        benchmark::DoNotOptimize(out);
    }
    s.SetItemsProcessed(n * s.iterations());
    s.SetBytesProcessed(n * 2 * sizeof(uint16_t) * s.iterations());
}
BENCHMARK(bench);

void scalar_convolve(uint16_t const *__restrict in, uint16_t *__restrict out, uint16_t const *__restrict filter, size_t n, uint8_t shift) {
    for (size_t i = 0; i < n; i++) {
        uint16_t tmp = 0;
        for (size_t j = 0; j < 4; j++) {
            tmp += in[i + j] * filter[j];
        }
        out[i] = tmp >> shift;
    }
}

TEST(MySuite, MyTest) {
    const auto n = size_t(32768);
    auto in = std::vector<uint16_t>(n + 4);
    auto out = std::vector<uint16_t>(n);
    auto out2 = std::vector<uint16_t>(n);
    auto filter = std::vector<uint16_t>{1, 2, 3, 4};
    std::generate(in.begin(), in.end(), [uni = std::uniform_int_distribution<uint16_t>(0, 100), rng = std::mt19937()] () mutable { return uni(rng); });
    convolve(in.data(), out.data(), filter.data(), n, 2);
    scalar_convolve(in.data(), out2.data(), filter.data(), n, 2);
    for (size_t i = 0; i < n; i++) {
        EXPECT_EQ(out[i], out2[i]);
        if (out[i] != out2[i]) break;
    }
}
