#include <cstdio>
#include <experimental/simd>
#include <benchmark/benchmark.h>
#include <random>
#include <algorithm>
#include <cmath>
#include "wangshash.h"
#include "print.h"

namespace stdx = std::experimental;

void scalar_fillsin(std::vector<float> &a) {
    for (size_t i = 0; i < a.size(); i++) {
        a[i] = std::sin(i);
    }
}

void simd_fillsin(std::vector<float> &a) {
    size_t i;
    std::array<int, stdx::simd<float>::size()> indices;
    std::iota(indices.begin(), indices.end(), 0);
    stdx::simd<float> v_i;
    v_i.copy_from(&indices[0], stdx::element_aligned);
    for (i = 0; i + stdx::simd<float>::size() - 1 < a.size(); i += stdx::simd<float>::size()) {
        stdx::simd<float> v_a;
        v_a = stdx::sin(v_i);
        v_a.copy_to(&a[i], stdx::element_aligned);
        v_i += (float)stdx::simd<float>::size();
    }
    for (; i < a.size(); i++) {
        a[i] = std::sin(i);
    }
}

constexpr size_t n = 65536;

void BM_simd_fillsin(benchmark::State &s) {
    std::vector<float> a(n);
    std::generate(a.begin(), a.end(),
                  [rng = wangshash(), uni = std::uniform_real_distribution<float>(-100, 100)]
                  () mutable { return uni(rng); });
    for (auto _: s) {
        simd_fillsin(a);
        benchmark::DoNotOptimize(a);
    }
}
BENCHMARK(BM_simd_fillsin);

void BM_scalar_fillsin(benchmark::State &s) {
    std::vector<float> a(n);
    std::generate(a.begin(), a.end(),
                  [rng = wangshash(), uni = std::uniform_real_distribution<float>(-100, 100)]
                  () mutable { return uni(rng); });
    for (auto _: s) {
        scalar_fillsin(a);
        benchmark::DoNotOptimize(a);
    }
}
BENCHMARK(BM_scalar_fillsin);
