#pragma once

#include <cstdint>
#include <cstddef>

struct xorshift32 { // suitable for serial multi-shot use
    uint32_t a;

    explicit xorshift32(size_t seed = 0) : a(static_cast<uint32_t>(seed + 1)) {
    }

    using result_type = uint32_t;

    constexpr uint32_t operator()() noexcept {
        uint32_t x = a;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        return a = x;
    }

    static constexpr uint32_t min() noexcept {
        return 1;
    }

    static constexpr uint32_t max() noexcept {
        return UINT32_MAX;
    }
};

struct wangshash { // suitable for parallel one-shot use
    uint32_t a;

    explicit wangshash(size_t seed = 0) : a(static_cast<uint32_t>(seed)) {
    }

    using result_type = uint32_t;

    constexpr uint32_t operator()() noexcept {
        uint32_t x = a;
        x = (x ^ 61) ^ (x >> 16);
        x *= 9;
        x = x ^ (x >> 4);
        x *= 0x27d4eb2d;
        x = x ^ (x >> 15);
        return a = x;
    }

    static constexpr uint32_t min() noexcept {
        return 0;
    }

    static constexpr uint32_t max() noexcept {
        return UINT32_MAX;
    }
};

/* #include <cstdio> */
/* #include <cstdlib> */
/* #include <ctime> */
/* #include <random> */
/* #include <algorithm> */
/* #include <thread> */
/* int main() { */
/*     std::vector<int> a; */
/*     a.reserve(100); */
/*     std::generate_n(std::back_inserter(a), 100, */
/*                     [rng = wangshash(0), uni = std::uniform_int_distribution<int>(0, 100)] */
/*                     () mutable { return uni(rng); }); */
/*     for (size_t i = 0; i < a.size(); i++) { */
/*         printf("%d\n", a[i]); */
/*     } */
/*     return 0; */
/* } */
