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
