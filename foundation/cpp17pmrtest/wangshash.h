#pragma once

#include <cstdint>
#include <cstddef>

struct wangshash {
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
