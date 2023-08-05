#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include <random>
#include <memory>
#include <cstddef>
#include <immintrin.h>

// BEGIN CODE
void imagemask(uint8_t const *img, uint8_t const *mask, size_t n, uint8_t *out) {
    for (size_t i = 0; i < n; i++) {
        out[i * 3 + 0] = img[i * 3 + 0] & mask[i];
        out[i * 3 + 1] = img[i * 3 + 1] & mask[i];
        out[i * 3 + 2] = img[i * 3 + 2] & mask[i];
    }
}
// END CODE

static void bench(benchmark::State &s) {
    const auto n = size_t(8192);
    auto img = std::vector<uint8_t>(n * 3);
    auto mask = std::vector<uint8_t>(n);
    auto out = std::vector<uint8_t>(n * 3);
    auto out2 = std::vector<uint8_t>(n * 3);
    std::generate(img.begin(), img.end(), [uni = std::uniform_real_distribution<float>(), rng = std::mt19937()] () mutable { return uni(rng); });
    std::generate(mask.begin(), mask.end(), [uni = std::uniform_real_distribution<float>(), rng = std::mt19937()] () mutable { return uni(rng); });
    for (auto _: s) {
        imagemask(img.data(), mask.data(), n, out.data());
        benchmark::DoNotOptimize(img);
        benchmark::DoNotOptimize(mask);
        benchmark::DoNotOptimize(out);
    }
    s.SetItemsProcessed(n * s.iterations());
}
BENCHMARK(bench);

void scalar_imagemask(uint8_t const *img, uint8_t const *mask, size_t n, uint8_t *out) {
    for (size_t i = 0; i < n; i++) {
        out[i * 3 + 0] = img[i * 3 + 0] & mask[i];
        out[i * 3 + 1] = img[i * 3 + 1] & mask[i];
        out[i * 3 + 2] = img[i * 3 + 2] & mask[i];
    }
}

TEST(MySuite, MyTest) {
    const auto n = size_t(16384);
    auto img = std::vector<uint8_t>(n * 3);
    auto mask = std::vector<uint8_t>(n);
    auto out = std::vector<uint8_t>(n * 3);
    auto out2 = std::vector<uint8_t>(n * 3);
    std::generate(img.begin(), img.end(), [uni = std::uniform_real_distribution<float>(), rng = std::mt19937()] () mutable { return uni(rng); });
    std::generate(mask.begin(), mask.end(), [uni = std::uniform_real_distribution<float>(), rng = std::mt19937()] () mutable { return uni(rng); });
    imagemask(img.data(), mask.data(), n, out.data());
    scalar_imagemask(img.data(), mask.data(), n, out2.data());
    for (size_t i = 0; i < n * 3; i++) {
        EXPECT_EQ(out[i], out2[i]);
        if (out[i] != out2[i]) break;
    }
}
