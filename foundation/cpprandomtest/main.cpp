#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <random>
#include <algorithm>
#include <iostream>
#include <numeric>

struct xorshift32 {
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

int main() {
    std::vector<float> gailv = {0.6f, 0.25f, 0.05f, 0.1f};
    std::vector<float> gailv_scanned;
    std::inclusive_scan(gailv.begin(), gailv.end(), std::back_inserter(gailv_scanned));
    for (size_t i = 0; i < gailv_scanned.size(); i++) {
        printf("%f\n", gailv_scanned[i]);
    }
    std::vector<std::string> lian = {"mushi", "she", "enlosi", "wuya"};
    std::mt19937 rng{std::random_device{}()};
    std::uniform_real_distribution<float> unf(0.0f, 1.0f);
    auto genlian = [&] () -> std::string {
        float f = unf(rng);
        auto it = std::lower_bound(gailv_scanned.begin(), gailv_scanned.end(), f);
        if (it == gailv_scanned.end()) return "";
        return lian[it - gailv_scanned.begin()];
    };
    std::cout << genlian() << '\n';
    std::cout << genlian() << '\n';
    std::cout << genlian() << '\n';
    std::cout << genlian() << '\n';
    std::cout << genlian() << '\n';
    std::cout << genlian() << '\n';
    std::cout << genlian() << '\n';
    std::cout << genlian() << '\n';
    std::cout << genlian() << '\n';
    std::cout << genlian() << '\n';
    std::cout << genlian() << '\n';
    std::cout << genlian() << '\n';
    std::cout << genlian() << '\n';
    std::cout << genlian() << '\n';
    std::cout << genlian() << '\n';
    return 0;
}
