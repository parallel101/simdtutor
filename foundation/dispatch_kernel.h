#pragma once

#include <cstddef>
#include <cstdint>

#if defined(BENCHMARK_DISPATCHED_KERNELS) && defined(__GNUC__) && defined(__x86_64__)
#include <tuple>
#include <cstdio>
#include <random>
#include <vector>
#include <limits>
#include <cmath>
#include <cstring>
#include <algorithm>
#include <type_traits>
#include <benchmark/benchmark.h>
#include <immintrin.h>

#define RESTRICT __restrict
#define _DISPATCH_KERNEL_BENCHMARK(__benchname, __bmfuncname, __funcname) \
    static int __dispatch_benchmark_register_helper_##__bmfuncname = ((void) \
        __dispatch_benchmark_pointer_list_##__funcname.emplace_back( \
            ::benchmark::internal::RegisterBenchmarkInternal( \
                new ::benchmark::internal::FunctionBenchmark(__benchname, __bmfuncname))), \
        0);
#define DISPATCH_KERNEL_BEGIN(__rettype, __funcname, ...) \
    static std::vector<::benchmark::internal::Benchmark *> __dispatch_benchmark_pointer_list_##__funcname; \
    static void __dispatch_bench_case_##__funcname(::benchmark::State &, void *); \
    static __rettype __dispatch_for_bench_##__funcname##_default __VA_ARGS__ \
    static void _BM_##__funcname##_default(::benchmark::State &__benchstate) { \
        __dispatch_bench_case_##__funcname(__benchstate, (void *)__dispatch_for_bench_##__funcname##_default); \
    } \
    _DISPATCH_KERNEL_BENCHMARK(#__funcname "@default", _BM_##__funcname##_default, __funcname);
#define DISPATCH_KERNEL(...) _DISPATCH_KERNEL(__LINE__, __VA_ARGS__)
#define _DISPATCH_KERNEL(...) __DISPATCH_KERNEL(__VA_ARGS__)
#define __DISPATCH_KERNEL(__line, __targets, __rettype, __funcname, ...) \
    __attribute__((__target__(__targets))) static __rettype __dispatch_for_bench_##__funcname##_simd_##__line __VA_ARGS__ \
    static void _BM_##__funcname##_simd_##__line(::benchmark::State &__benchstate) { \
        static_assert(std::is_same<decltype(+__dispatch_for_bench_##__funcname##_default), decltype(+__dispatch_for_bench_##__funcname##_simd_##__line)>::value, "function argument list mismatch"); \
        if (__dispatch_benchmark_details::test_cpu_supports_all(__targets)) { \
             __dispatch_bench_case_##__funcname(__benchstate, (void *)__dispatch_for_bench_##__funcname##_simd_##__line); \
        } else { \
            __benchstate.SkipWithMessage("not supported by cpu: " __targets); \
        } \
    } \
    _DISPATCH_KERNEL_BENCHMARK(#__funcname "@" __targets, _BM_##__funcname##_simd_##__line, __funcname);
#define DISPATCH_KERNEL_DEBUG(...) _DISPATCH_KERNEL_DEBUG(__LINE__, __VA_ARGS__)
#define _DISPATCH_KERNEL_DEBUG(...) __DISPATCH_KERNEL_DEBUG(__VA_ARGS__)
#define __DISPATCH_KERNEL_DEBUG(__line, __nonprodhint, __targets, __rettype, __funcname, ...) \
    __attribute__((__target__(__targets))) static __rettype __dispatch_for_bench_##__funcname##_simd_##__line __VA_ARGS__ \
    static void _BM_##__funcname##_simd_##__line(::benchmark::State &__benchstate) { \
        static_assert(std::is_same<decltype(+__dispatch_for_bench_##__funcname##_default), \
                        decltype(+__dispatch_for_bench_##__funcname##_simd_##__line)>::value, \
                        "function argument list mismatch"); \
        if (__builtin_cpu_supports(__targets)) { \
             __dispatch_bench_case_##__funcname(__benchstate, (void *)__dispatch_for_bench_##__funcname##_simd_##__line); \
        } else { \
            __benchstate.SkipWithMessage("not supported by cpu: " __targets); \
        } \
    } \
    _DISPATCH_KERNEL_BENCHMARK(#__funcname "@" __targets "~" __nonprodhint, _BM_##__funcname##_simd_##__line, __funcname);
#define DISPATCH_KERNEL_END(__rettype, __funcname, __funcargs, ...) \
    static void __dispatch_bench_case_##__funcname(::benchmark::State &__benchstate, void *__benchfptr) { \
        typedef __rettype (*__benchfp_t) __funcargs; \
        static_assert(std::is_same<decltype(+__dispatch_for_bench_##__funcname##_default), \
            __benchfp_t>::value, "function argument list mismatch"); \
        if (__VA_OPT__(!) 0) { \
            __VA_OPT__(_DISPATCH_KERNEL_AUTO_BENCHMARK(__funcname, __VA_ARGS__)) \
        } else { \
            __benchstate.SkipWithMessage("no benchmark case defined"); \
        } \
    } __VA_OPT__(_DISPATCH_KERNEL_REGISTER_FINALIZER(__funcname, __VA_ARGS__))
#define _DISPATCH_KERNEL_AUTO_BENCHMARK(__funcname, ...) \
    const size_t n = __benchstate.range(0); \
    const struct __notest_t { typedef void __yes_i_am_notest; } notest; \
    auto bpi = [] (size_t __bpi) { return __bpi; }; \
    ::__dispatch_benchmark_details::AutoBenchmark<__benchfp_t> __autobench( \
            __benchstate, __dispatch_for_bench_##__funcname##_default, (__benchfp_t)__benchfptr, n, __VA_ARGS__);
#define _DISPATCH_KERNEL_REGISTER_FINALIZER(__funcname, ...) \
    static int __dispatch_benchmark_register_finalizer_##__funcname = ((void) [] () { \
        for (auto __it = __dispatch_benchmark_pointer_list_##__funcname.begin(); \
             __it != __dispatch_benchmark_pointer_list_##__funcname.end(); \
             ++__it) { (*__it)->DISPATCH_BENCHMARK_RANGE; } \
    } (), 0);
#ifndef DISPATCH_BENCHMARK_RANGE
#define DISPATCH_BENCHMARK_RANGE Arg(1024*768+3)
#endif

namespace __dispatch_benchmark_details {

static inline bool test_cpu_supports_all(const char *t) {
    static const std::map<std::string, bool> __targettab = {
        {"mmx", __builtin_cpu_supports("mmx")},
        {"sse", __builtin_cpu_supports("sse")},
        {"sse2", __builtin_cpu_supports("sse2")},
        {"sse3", __builtin_cpu_supports("sse3")},
        {"ssse3", __builtin_cpu_supports("ssse3")},
        {"sse4.1", __builtin_cpu_supports("sse4.1")},
        {"sse4.2", __builtin_cpu_supports("sse4.2")},
        {"sse4a", __builtin_cpu_supports("sse4a")},
        {"bmi", __builtin_cpu_supports("bmi")},
        {"bmi2", __builtin_cpu_supports("bmi2")},
        {"avx", __builtin_cpu_supports("avx")},
        {"avx2", __builtin_cpu_supports("avx2")},
        {"fma", __builtin_cpu_supports("fma")},
        {"avx512f", __builtin_cpu_supports("avx512f")},
        {"avx512pf", __builtin_cpu_supports("avx512pf")},
        {"avx512er", __builtin_cpu_supports("avx512er")},
        {"avx512cd", __builtin_cpu_supports("avx512cd")},
        {"popcnt", __builtin_cpu_supports("popcnt")},
        {"cmov", __builtin_cpu_supports("cmov")},
        {"aes", __builtin_cpu_supports("aes")},
        {"pclmul", __builtin_cpu_supports("pclmul")},
        {"xop", __builtin_cpu_supports("xop")},
    };
    while (true) {
        auto p = strchr(t, ',');
        auto it = __targettab.find(p ? std::string(t, p - t) : std::string(t));
        if (it == __targettab.end() || !it->second) return false;
        if (!p) break;
        t = p + 1;
    }
    return true;
}

template <class T>
static typename std::enable_if<std::is_integral<typename
    std::decay<T>::type::value_type>::value>::type randomize_test_data(T &t) {
    std::generate(t.begin(), t.end(), [uni =
                    std::uniform_int_distribution<typename std::decay<T>::type::value_type>(),
                    rng = std::mt19937()] () mutable { return uni(rng); });
}

template <class T>
static typename std::enable_if<std::is_floating_point<typename
    std::decay<T>::type::value_type>::value>::type randomize_test_data(T &t) {
    std::generate(t.begin(), t.end(), [uni =
                    std::uniform_real_distribution<typename std::decay<T>::type::value_type>(),
                    rng = std::mt19937()] () mutable { return uni(rng); });
}

template <class T>
static typename std::enable_if<std::is_floating_point<T>::value, bool>::type test_equalness(T const &lhs, T const &rhs) {
    if (std::isnan(lhs) || std::isnan(rhs)) return false;
    else if (std::isinf(lhs)) return std::isinf(rhs);
    else if (std::isinf(rhs)) return std::isinf(lhs);
    T delta = std::abs(lhs - rhs);
    T sum = lhs + rhs + std::numeric_limits<T>::epsilon();
    return delta <= sum * std::numeric_limits<T>::epsilon();
}

template <class T>
static typename std::enable_if<!std::is_floating_point<T>::value, bool>::type test_equalness(T const &lhs, T const &rhs) {
    return lhs == rhs;
}

template <class T>
struct BenchStore {
    T inner;

    BenchStore(T val) : inner(std::move(val)) {
    }

    T data() {
        benchmark::DoNotOptimize(inner);
        return inner;
    }

    std::string match(BenchStore &that) {
        return {};
    }
};

template <class T>
struct BenchStore<T *> {
    std::vector<T> inner;

    BenchStore(size_t n) {
        inner.clear();
        inner.resize(n);
    }

    T *data() {
        benchmark::DoNotOptimize(inner);
        return inner.data();
    }

    std::string match(BenchStore &that) {
        if (inner.size() != that.inner.size())
            return "expect size " + std::to_string(that.inner.size())
                + " got size " + std::to_string(inner.size());
        for (size_t i = 0; i < inner.size(); i++) {
            if (!test_equalness(inner[i], that.inner[i]))
                return "at " + std::to_string(i) + " expect "
                    + std::to_string(that.inner[i]) + " got " + std::to_string(inner[i]);
        }
        return {};
    }
};

template <class T>
struct BenchStore<T const *> {
    std::vector<T> inner;

    BenchStore(size_t n) {
        inner.resize(n);
        randomize_test_data(inner);
    }

    T const *data() {
        benchmark::DoNotOptimize(inner);
        return inner.data();
    }

    std::string match(BenchStore &that) {
        return {};
    }
};

struct BenchTestTag {
    explicit BenchTestTag() = default;
};

template <class F>
struct AutoBenchmark {
    template <class U, class V = int>
    explicit AutoBenchmark(::benchmark::State &s, F stdfp, F fp, size_t n, U &&, size_t bpi = 0, V = 0) {
        s.SkipWithError("not implemented");
    }
};

template <class R, class ...Ts>
struct AutoBenchmark<R(*)(Ts...)> {
    bool tested = false;

    template <size_t ...Is>
    void _impl_bench(::benchmark::State &s, R (*stdfp)(Ts...), R (*fp)(Ts...), size_t n,
                     std::tuple<BenchStore<typename std::decay<Ts>::type>...> &stores,
                     size_t bpi, std::index_sequence<Is...>) {
        if (!tested) {
            auto stdstores = stores;
            R stdret = stdfp(std::get<Is>(stdstores).data()...);
            R ret = fp(std::get<Is>(stores).data()...);
            if (!test_equalness(stdret, ret)) {
                s.SkipWithError("test failed: return value mismatch (expect "
                                + std::to_string(stdret) + " got " + std::to_string(ret) + ")");
            }
            std::array<std::string, sizeof...(Ts)> matches{std::get<Is>(stores).match(std::get<Is>(stdstores))...};
            for (size_t i = 0; i < matches.size(); i++) {
                if (!matches[i].empty()) {
                    s.SkipWithError("test failed: value mismatch (" + matches[i] + ")");
                }
            }
            tested = true;
        }
        for (auto _: s) {
            R ret = fp(std::get<Is>(stores).data()...);
            benchmark::DoNotOptimize(ret);
        }
        if (bpi) s.SetBytesProcessed(bpi * n * s.iterations());
        else s.SetItemsProcessed(n * s.iterations());
    }

    explicit AutoBenchmark(::benchmark::State &s, R (*stdfp)(Ts...), R (*fp)(Ts...), size_t n,
                           std::tuple<BenchStore<typename std::decay<Ts>::type>...> stores,
                           size_t bpi = 0) {
        _impl_bench(s, stdfp, fp, n, stores, bpi, std::make_index_sequence<sizeof...(Ts)>{});
    }

    template <class BPI, class NoTest, class = typename NoTest::__yes_i_am_notest>
    explicit AutoBenchmark(::benchmark::State &s, R (*stdfp)(Ts...), void (*fp)(Ts...), size_t n,
                           std::tuple<BenchStore<typename std::decay<Ts>::type>...> stores,
                           size_t bpi, NoTest) {
        tested = true;
        _impl_bench(s, stdfp, fp, n, stores, bpi, std::make_index_sequence<sizeof...(Ts)>{});
    }

    template <class BPI, class NoTest, class = typename NoTest::__yes_i_am_notest>
    explicit AutoBenchmark(::benchmark::State &s, R (*stdfp)(Ts...), void (*fp)(Ts...), size_t n,
                           std::tuple<BenchStore<typename std::decay<Ts>::type>...> stores,
                           NoTest, size_t bpi = 0) {
        tested = true;
        _impl_bench(s, stdfp, fp, n, stores, bpi, std::make_index_sequence<sizeof...(Ts)>{});
    }
};

template <class ...Ts>
struct AutoBenchmark<void(*)(Ts...)> {
    bool tested = false;

    template <size_t ...Is>
    void _impl_bench(::benchmark::State &s, void (*stdfp)(Ts...), void (*fp)(Ts...), size_t n,
                     std::tuple<BenchStore<typename std::decay<Ts>::type>...> &stores,
                     size_t bpi, std::index_sequence<Is...>) {
        if (!tested) {
            auto stdstores = stores;
            stdfp(std::get<Is>(stdstores).data()...);
            fp(std::get<Is>(stores).data()...);
            std::array<std::string, sizeof...(Ts)> matches{std::get<Is>(stores).match(std::get<Is>(stdstores))...};
            for (size_t i = 0; i < matches.size(); i++) {
                if (!matches[i].empty()) {
                    s.SkipWithError("test failed: value mismatch (" + matches[i] + ")");
                }
            }
            tested = true;
        }
        for (auto _: s) {
            fp(std::get<Is>(stores).data()...);
        }
        if (bpi) s.SetBytesProcessed(bpi * n * s.iterations());
        else s.SetItemsProcessed(n * s.iterations());
    }

    explicit AutoBenchmark(::benchmark::State &s, void (*stdfp)(Ts...), void (*fp)(Ts...), size_t n,
                           std::tuple<BenchStore<typename std::decay<Ts>::type>...> stores,
                           size_t bpi = 0) {
        _impl_bench(s, stdfp, fp, n, stores, bpi, std::make_index_sequence<sizeof...(Ts)>{});
    }

    template <class NoTest, class = typename NoTest::__yes_i_am_notest>
    explicit AutoBenchmark(::benchmark::State &s, void (*stdfp)(Ts...), void (*fp)(Ts...), size_t n,
                           std::tuple<BenchStore<typename std::decay<Ts>::type>...> stores,
                           size_t bpi, NoTest) {
        tested = true;
        _impl_bench(s, stdfp, fp, n, stores, bpi, std::make_index_sequence<sizeof...(Ts)>{});
    }

    template <class NoTest, class = typename NoTest::__yes_i_am_notest>
    explicit AutoBenchmark(::benchmark::State &s, void (*stdfp)(Ts...), void (*fp)(Ts...), size_t n,
                           std::tuple<BenchStore<typename std::decay<Ts>::type>...> stores,
                           NoTest, size_t bpi = 0) {
        tested = true;
        _impl_bench(s, stdfp, fp, n, stores, bpi, std::make_index_sequence<sizeof...(Ts)>{});
    }
};

}

#elif defined(__GNUC__) && defined(__x86_64__)
#include <immintrin.h>

#define RESTRICT __restrict
#ifdef NDEBUG
#define DISPATCH_KERNEL_BEGIN(__rettype, __funcname, ...) \
    __attribute__((__target__("default"))) static __rettype __funcname __VA_ARGS__
#define DISPATCH_KERNEL(__targets, __rettype, __funcname, ...) \
    __attribute__((__target__(__targets))) static __rettype __funcname __VA_ARGS__
#define DISPATCH_KERNEL_DEBUG(__nonprodhint, __target, __rettype, __funcname, ...)
#define DISPATCH_KERNEL_END(__rettype, __funcname, __funcargs, ...)
#else
#include <type_traits>
#define DISPATCH_KERNEL_BEGIN(__rettype, __funcname, ...) \
    __attribute__((__target__("default"))) static __rettype __funcname __VA_ARGS__
#define DISPATCH_KERNEL(__targets, __rettype, __funcname, ...) \
    __attribute__((__target__(__targets))) static __rettype __funcname __VA_ARGS__
#define DISPATCH_KERNEL_DEBUG(__nonprodhint, __target, __rettype, __funcname, ...)
#define DISPATCH_KERNEL_END(__rettype, __funcname, __funcargs, ...) \
    static_assert(std::is_same<__rettype (*) __funcargs, decltype(+__funcname)>::value, "function argument list mismatch");
#warning "building in debug mode"
#endif

#elif defined(_MSC_VER) && defined(_M_X64) && _MSC_FULL_VER >= 160040219
#include <intrin.h>
#include <immintrin.h>
#include <type_traits>
#include <vector>
#include <bitset>
#include <array>
#include <string>
#include <cstring>
#include <cstdio>
#include <map>

class MSVC_CPUID
{ // https://learn.microsoft.com/zh-cn/cpp/intrinsics/cpuid-cpuidex?view=msvc-170
    // forward declarations
    class Internal;

public:
    // getters
    static std::string Vendor(void) { return CPU_Rep.vendor_; }
    static std::string Brand(void) { return CPU_Rep.brand_; }

    static bool SSE3(void) { return CPU_Rep.f_1_ECX_[0]; }
    static bool PCLMULQDQ(void) { return CPU_Rep.f_1_ECX_[1]; }
    static bool MONITOR(void) { return CPU_Rep.f_1_ECX_[3]; }
    static bool SSSE3(void) { return CPU_Rep.f_1_ECX_[9]; }
    static bool FMA(void) { return CPU_Rep.f_1_ECX_[12]; }
    static bool CMPXCHG16B(void) { return CPU_Rep.f_1_ECX_[13]; }
    static bool SSE41(void) { return CPU_Rep.f_1_ECX_[19]; }
    static bool SSE42(void) { return CPU_Rep.f_1_ECX_[20]; }
    static bool MOVBE(void) { return CPU_Rep.f_1_ECX_[22]; }
    static bool POPCNT(void) { return CPU_Rep.f_1_ECX_[23]; }
    static bool AES(void) { return CPU_Rep.f_1_ECX_[25]; }
    static bool XSAVE(void) { return CPU_Rep.f_1_ECX_[26]; }
    static bool OSXSAVE(void) { return CPU_Rep.f_1_ECX_[27]; }
    static bool AVX(void) { return CPU_Rep.f_1_ECX_[28]; }
    static bool F16C(void) { return CPU_Rep.f_1_ECX_[29]; }
    static bool RDRAND(void) { return CPU_Rep.f_1_ECX_[30]; }

    static bool MSR(void) { return CPU_Rep.f_1_EDX_[5]; }
    static bool CX8(void) { return CPU_Rep.f_1_EDX_[8]; }
    static bool SEP(void) { return CPU_Rep.f_1_EDX_[11]; }
    static bool CMOV(void) { return CPU_Rep.f_1_EDX_[15]; }
    static bool CLFSH(void) { return CPU_Rep.f_1_EDX_[19]; }
    static bool MMX(void) { return CPU_Rep.f_1_EDX_[23]; }
    static bool FXSR(void) { return CPU_Rep.f_1_EDX_[24]; }
    static bool SSE(void) { return CPU_Rep.f_1_EDX_[25]; }
    static bool SSE2(void) { return CPU_Rep.f_1_EDX_[26]; }

    static bool FSGSBASE(void) { return CPU_Rep.f_7_EBX_[0]; }
    static bool BMI1(void) { return CPU_Rep.f_7_EBX_[3]; }
    static bool HLE(void) { return CPU_Rep.isIntel_ && CPU_Rep.f_7_EBX_[4]; }
    static bool AVX2(void) { return CPU_Rep.f_7_EBX_[5]; }
    static bool BMI2(void) { return CPU_Rep.f_7_EBX_[8]; }
    static bool ERMS(void) { return CPU_Rep.f_7_EBX_[9]; }
    static bool INVPCID(void) { return CPU_Rep.f_7_EBX_[10]; }
    static bool RTM(void) { return CPU_Rep.isIntel_ && CPU_Rep.f_7_EBX_[11]; }
    static bool AVX512F(void) { return CPU_Rep.f_7_EBX_[16]; }
    static bool RDSEED(void) { return CPU_Rep.f_7_EBX_[18]; }
    static bool ADX(void) { return CPU_Rep.f_7_EBX_[19]; }
    static bool AVX512PF(void) { return CPU_Rep.f_7_EBX_[26]; }
    static bool AVX512ER(void) { return CPU_Rep.f_7_EBX_[27]; }
    static bool AVX512CD(void) { return CPU_Rep.f_7_EBX_[28]; }
    static bool SHA(void) { return CPU_Rep.f_7_EBX_[29]; }

    static bool PREFETCHWT1(void) { return CPU_Rep.f_7_ECX_[0]; }

    static bool LAHF(void) { return CPU_Rep.f_81_ECX_[0]; }
    static bool LZCNT(void) { return CPU_Rep.isIntel_ && CPU_Rep.f_81_ECX_[5]; }
    static bool ABM(void) { return CPU_Rep.isAMD_ && CPU_Rep.f_81_ECX_[5]; }
    static bool SSE4a(void) { return CPU_Rep.isAMD_ && CPU_Rep.f_81_ECX_[6]; }
    static bool XOP(void) { return CPU_Rep.isAMD_ && CPU_Rep.f_81_ECX_[11]; }
    static bool TBM(void) { return CPU_Rep.isAMD_ && CPU_Rep.f_81_ECX_[21]; }

    static bool SYSCALL(void) { return CPU_Rep.isIntel_ && CPU_Rep.f_81_EDX_[11]; }
    static bool MMXEXT(void) { return CPU_Rep.isAMD_ && CPU_Rep.f_81_EDX_[22]; }
    static bool RDTSCP(void) { return CPU_Rep.isIntel_ && CPU_Rep.f_81_EDX_[27]; }
    static bool _3DNOWEXT(void) { return CPU_Rep.isAMD_ && CPU_Rep.f_81_EDX_[30]; }
    static bool _3DNOW(void) { return CPU_Rep.isAMD_ && CPU_Rep.f_81_EDX_[31]; }

private:
    class Internal
    {
    public:
        Internal()
            : nIds_{ 0 },
            nExIds_{ 0 },
            isIntel_{ false },
            isAMD_{ false },
            f_1_ECX_{ 0 },
            f_1_EDX_{ 0 },
            f_7_EBX_{ 0 },
            f_7_ECX_{ 0 },
            f_81_ECX_{ 0 },
            f_81_EDX_{ 0 },
            data_{},
            extdata_{}
        {
            //int cpuInfo[4] = {-1};
            std::array<int, 4> cpui;

            // Calling __cpuid with 0x0 as the function_id argument
            // gets the number of the highest valid function ID.
            __cpuid(cpui.data(), 0);
            nIds_ = cpui[0];

            for (int i = 0; i <= nIds_; ++i)
            {
                __cpuidex(cpui.data(), i, 0);
                data_.push_back(cpui);
            }

            // Capture vendor string
            char vendor[0x20];
            memset(vendor, 0, sizeof(vendor));
            *reinterpret_cast<int*>(vendor) = data_[0][1];
            *reinterpret_cast<int*>(vendor + 4) = data_[0][3];
            *reinterpret_cast<int*>(vendor + 8) = data_[0][2];
            vendor_ = vendor;
            if (vendor_ == "GenuineIntel")
            {
                isIntel_ = true;
            }
            else if (vendor_ == "AuthenticAMD")
            {
                isAMD_ = true;
            }

            // load bitset with flags for function 0x00000001
            if (nIds_ >= 1)
            {
                f_1_ECX_ = data_[1][2];
                f_1_EDX_ = data_[1][3];
            }

            // load bitset with flags for function 0x00000007
            if (nIds_ >= 7)
            {
                f_7_EBX_ = data_[7][1];
                f_7_ECX_ = data_[7][2];
            }

            // Calling __cpuid with 0x80000000 as the function_id argument
            // gets the number of the highest valid extended ID.
            __cpuid(cpui.data(), 0x80000000);
            nExIds_ = cpui[0];

            char brand[0x40];
            memset(brand, 0, sizeof(brand));

            for (int i = 0x80000000; i <= nExIds_; ++i)
            {
                __cpuidex(cpui.data(), i, 0);
                extdata_.push_back(cpui);
            }

            // load bitset with flags for function 0x80000001
            if (nExIds_ >= 0x80000001)
            {
                f_81_ECX_ = extdata_[1][2];
                f_81_EDX_ = extdata_[1][3];
            }

            // Interpret CPU brand string if reported
            if (nExIds_ >= 0x80000004)
            {
                memcpy(brand, extdata_[2].data(), sizeof(cpui));
                memcpy(brand + 16, extdata_[3].data(), sizeof(cpui));
                memcpy(brand + 32, extdata_[4].data(), sizeof(cpui));
                brand_ = brand;
            }
        };

        int nIds_;
        int nExIds_;
        std::string vendor_;
        std::string brand_;
        bool isIntel_;
        bool isAMD_;
        std::bitset<32> f_1_ECX_;
        std::bitset<32> f_1_EDX_;
        std::bitset<32> f_7_EBX_;
        std::bitset<32> f_7_ECX_;
        std::bitset<32> f_81_ECX_;
        std::bitset<32> f_81_EDX_;
        std::vector<std::array<int, 4>> data_;
        std::vector<std::array<int, 4>> extdata_;
    };

    static inline const Internal CPU_Rep;
};

inline void *__msvc_simd_do_dispatch(std::vector<std::pair<const char *, void *>> const &__disptab, void *__defaultfp) {
    static const std::map<std::string, bool> __targettab = {
        // https://learn.microsoft.com/zh-CN/windows/win32/api/processthreadsapi/nf-processthreadsapi-isprocessorfeaturepresent
        {"mmx", MSVC_CPUID::MMX()},
        {"sse", MSVC_CPUID::SSE()},
        {"sse2", MSVC_CPUID::SSE2()},
        {"sse3", MSVC_CPUID::SSE3()},
        {"ssse3", MSVC_CPUID::SSSE3()},
        {"sse4.1", MSVC_CPUID::SSE41()},
        {"sse4.2", MSVC_CPUID::SSE42()},
        {"sse4a", MSVC_CPUID::SSE4a()},
        {"bmi", MSVC_CPUID::BMI1()},
        {"bmi2", MSVC_CPUID::BMI2()},
        {"avx", MSVC_CPUID::AVX()},
        {"avx2", MSVC_CPUID::AVX2()},
        {"fma", MSVC_CPUID::FMA()},
        {"avx512f", MSVC_CPUID::AVX512F()},
        {"avx512pf", MSVC_CPUID::AVX512PF()},
        {"avx512er", MSVC_CPUID::AVX512ER()},
        {"avx512cd", MSVC_CPUID::AVX512CD()},
        {"prefetchwt1", MSVC_CPUID::PREFETCHWT1()},
        {"popcnt", MSVC_CPUID::POPCNT()},
        {"lzcnt", MSVC_CPUID::LZCNT()},
        {"rdseed", MSVC_CPUID::RDSEED()},
        {"rdrnd", MSVC_CPUID::RDRAND()},
        {"rtsc", MSVC_CPUID::RDTSCP()},
        {"rtm", MSVC_CPUID::RTM()},
        {"hle", MSVC_CPUID::HLE()},
        {"sha", MSVC_CPUID::SHA()},
        {"3dnow", MSVC_CPUID::_3DNOW()},
        {"3dnowa", MSVC_CPUID::_3DNOWEXT()},
        {"tbm", MSVC_CPUID::TBM()},
        {"abm", MSVC_CPUID::ABM()},
        {"f16c", MSVC_CPUID::F16C()},
        {"cmov", MSVC_CPUID::CMOV()},
        {"sahf", MSVC_CPUID::LAHF()},
        {"movbe", MSVC_CPUID::MOVBE()},
        {"clflushopt", MSVC_CPUID::CLFSH()},
        {"fxsr", MSVC_CPUID::FXSR()},
        {"fsgsbase", MSVC_CPUID::FSGSBASE()},
        {"aes", MSVC_CPUID::AES()},
        {"pclmul", MSVC_CPUID::PCLMULQDQ()},
        {"xop", MSVC_CPUID::XOP()},
        {"adx", MSVC_CPUID::ADX()},
        {"cx16", MSVC_CPUID::CMPXCHG16B()},
    };
    for (auto __dit = __disptab.rbegin(); __dit != __disptab.rend(); ++__dit) {
        const char *__req = __dit->first, *__nextp;
        bool ok = true;
        while (true) {
            __nextp = strchr(req, ',');
            auto __it = __targettab.find(__nextp ? std::string(__req, __nextp - __req) : std::string(__req));
            if (__it == __targettab.end() || !__it->second) {
                ok = false;
                break;
            }
            if (!__nextp) {
                break;
            }
            __req = __nextp + 1;
        }
        if (ok) {
            /* printf("choosen target: %s", __dit->first); */
            return __dit->__second;
        }
    }
    /* printf("choosen target: default"); */
    return __defaultfp;
}

#define RESTRICT __restrict
#define DISPATCH_KERNEL_BEGIN(__rettype, __funcname, ...) \
    static __rettype __msvc_simd_default_ver_##__funcname __VA_ARGS__ \
    static std::vector<std::pair<const char *, void *>> __msvc_simd_dispatch_table_##__funcname;
#define DISPATCH_KERNEL(__target, __rettype, __funcname, ...) \
    static __rettype __msvc_simd_##__target##_ver_##__funcname __VA_ARGS__ \
    static int __msvc_simd_register_helper_##__target##_ver_##__funcname = \
        ((void)__msvc_simd_dispatch_table_##__funcname.emplace_back( \
            __target, (void *)__msvc_simd_##__target##_ver_##__funcname), 0);
#define DISPATCH_KERNEL_DEBUG(__nonprodhint, __target, __rettype, __funcname, ...)
#define DISPATCH_KERNEL_END(__rettype, __funcname, __funcargs, ...) \
    static auto *const __funcname = (decltype(+__msvc_simd_default_ver_##__funcname)) \
        __msvc_simd_do_dispatch(__msvc_simd_dispatch_table_##__funcname, (void *)__msvc_simd_default_ver_##__funcname);

#else

#define RESTRICT
#define DISPATCH_KERNEL_BEGIN(__rettype, __funcname, ...) \
    static __rettype __funcname __VA_ARGS__
#define DISPATCH_KERNEL(__target, __rettype, __funcname, ...)
#define DISPATCH_KERNEL_DEBUG(__nonprodhint, __target, __rettype, __funcname, ...)
#define DISPATCH_KERNEL_END(__rettype, __funcname, __funcargs, ...)

#endif
