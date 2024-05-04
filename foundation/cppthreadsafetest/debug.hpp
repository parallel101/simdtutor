#pragma once
//
// debug.hpp - prints everything!
//
// Usage:
//   debug(), "my variable is", your_variable;
//
// Results in:
//   your_file.cpp:233:  my variable is {1, 2, 3}
//
// (suppose your_variable is an std::vector)
//
// **WARNING**:
//   debug() only works in `Debug` build! It is automatically disabled in `Release` build (we do this by checking whether the NDEBUG macro is defined).
//
//   This is a feature for convenience, e.g. you don't have to remove all the debug() sentences after debug done, simply switch to `Release` build and everything debug is gone, no runtime overhead! And when you need debug simply switch back to `Debug` build and everything debug() you written before is back in life.
//
//   If you are mean to use debug() even in `Release` build, please `#define DEBUG_LEVEL 1` before including this header file.
//
//
// Assertion check:
//   debug().check(some_variable) > 0;
//
// Will trigger a 'trap' interrupt (__debugbreak for MSVC and __builtin_trap for GCC, configurable, see below) for the debugger to catch when `some_variable > 0` is false, as well as printing human readable error message:
//   your_file.cpp:233:  assertion failed: 3 < 0
//
//
// After debugging complete, no need to busy removing all debug() calls! Simply:
//   #define NDEBUG
// would supress all debug() prints and assertion checks, completely no runtime overhead. For CMake or Visual Studio users, simply switch to `Release` build would supress debug() prints. Since they automatically define `NDEBUG` for you in `Release`, `RelWithDebInfo` and `MinSizeRel` build types.
//
//
// TL;DR: This is a useful debugging utility the C++ programmers had all dreamed of:
//
//   1. print using the neat comma syntax, easy-to-use
//   2. supports printing STL objects including string, vector, tuple, optional, variant, unique_ptr, type_info, and so on. (C++20 required)
//   3. just add a member method named `repr`, e.g. `std::string repr() { ... }` to support printing your custom class!
//   4. classes that are not supported to print will be shown in something like `[TypeName@0xdeadbeaf]` where 0xdeadbeaf is it's address.
//   5. highly configurable, customize the behaviour by defining the DEBUG_xxx macros (see below)
//   6. when debug done, supress all debug messages by simply `#define NDEBUG`, the whole library is disabled at compile-time, no runtime overhead
//   7. Thread safe, every line of message is always distinct, no annoying interleaving output rushing into console (typical experience when using cout)
//
//
// Here is a list of configurable macros, define them **before** including this header file to take effect:
//
// `#define DEBUG_LEVEL 0` (default when defined NDEBUG) - disable debug output, completely no runtime overhead
// `#define DEBUG_LEVEL 1` (default when !defined NDEBUG) - enable debug output, prints everything you asked to print
// `#define DEBUG_LEVEL 2` - enable debug output with detailed source code level information (requires source files readable)

// `#define DEBUG_DEFAULT_OUTPUT std::cerr` (default) - controls where to output the debug strings

// `#define DEBUG_PANIC_METHOD 0` - throws an runtime error with debug string as message when assertion failed
// `#define DEBUG_PANIC_METHOD 1` (default) - print the error message when assertion failed, then triggers a 'trap' interrupt, useful for debuggers to catch, if no debuggers attached, the program would terminate
// `#define DEBUG_PANIC_METHOD 2` - print the error message when assertion failed, and then call std::terminate
// `#define DEBUG_PANIC_METHOD 3` - print the error message when assertion failed, do not terminate, do not throw any exception

// `#define DEBUG_REPR_NAME repr` (default) - if an object x have a member function like `x.repr()` or a global function `repr` supporting `repr(x)`, then the value of `x.repr()` or `repr(x)` will be printed instead for this object

// `#define DEBUG_RANGE_BRACE "{}"` (default) - controls format for range-like objects (supporting begin(x) and end(x)) in "{1, 2, 3, ...}"
// `#define DEBUG_RANGE_COMMA ", "` (default) - ditto

// `#define DEBUG_TUPLE_BRACE "{}"` (default) - controls format for tuple-like objects (supporting std::tuple_size<X>) in "{1, 2, 3}"
// `#define DEBUG_TUPLE_COMMA ", "` (default) - ditto

// `#define DEBUG_MAGIC_ENUM magic_enum::enum_name` - enable printing enum in their name rather than value, if you have magic_enum.hpp

// `#define DEBUG_UNSIGNED_AS_HEXADECIMAL 0` (default) - print unsigned integers as decimal
// `#define DEBUG_UNSIGNED_AS_HEXADECIMAL 1` - print unsigned integers as hexadecimal
// `#define DEBUG_UNSIGNED_AS_HEXADECIMAL 2` - print unsigned integers as full-width hexadecimal

// `#define DEBUG_HEXADECIMAL_UPPERCASE 0` (default) - print hexadecimal values in lowercase (a-f)
// `#define DEBUG_HEXADECIMAL_UPPERCASE 1` - print hexadecimal values in uppercase (A-F)

// `#define DEBUG_SUPRESS_NON_ASCII 0` (default) - consider non-ascii characters in std::string as printable (e.g. UTF-8 encoded Chinese characters)
// `#define DEBUG_SUPRESS_NON_ASCII 1` - consider non-ascii characters in std::string as not printable (print them in e.g. '\xfe' instead)

// `#define DEBUG_SHOW_NULLOPT "nullopt"` (default) - controls how to print optional-like objects (supporting *x and (bool)x) when it is nullopt

// `#define DEBUG_NAMESPACE_BEGIN` (default) - expose debug in the global namespace
// `#define DEBUG_NAMESPACE_END` (default) - ditto

// `#define DEBUG_NAMESPACE_BEGIN namespace mydebugger {` (default) - expose debug in the the namespace `mydebugger`, and use it like `mydebugger::debug()`
// `#define DEBUG_NAMESPACE_END }` (default) - ditto

// `#define DEBUG_CLASS_NAME debug()` (default) - the default name for the debug class is `debug()`, you may define your custom name here



#ifndef DEBUG_LEVEL
#ifdef NDEBUG
#define DEBUG_LEVEL 0
#else
#define DEBUG_LEVEL 1
#endif
#endif

#ifndef DEBUG_REPR_NAME
#define DEBUG_REPR_NAME repr
#endif

#ifndef DEBUG_NAMESPACE_BEGIN
#define DEBUG_NAMESPACE_BEGIN
#endif

#ifndef DEBUG_NAMESPACE_END
#define DEBUG_NAMESPACE_END
#endif

#ifndef DEBUG_DEFAULT_OUTPUT
#define DEBUG_DEFAULT_OUTPUT std::cerr
#endif

#ifndef DEBUG_DEFAULT_ENABLED
#define DEBUG_DEFAULT_ENABLED true
#endif

#ifndef DEBUG_ENABLE_FILES_MATCH
#define DEBUG_ENABLE_FILES_MATCH 0
#endif

#ifndef DEBUG_PANIC_METHOD
#define DEBUG_PANIC_METHOD 1
#endif

#ifndef DEBUG_SUPRESS_NON_ASCII
#define DEBUG_SUPRESS_NON_ASCII 0
#endif

#ifndef DEBUG_SEPARATOR_TAB
#define DEBUG_SEPARATOR_TAB '\t'
#endif

#ifndef DEBUG_RANGE_BRACE
#define DEBUG_RANGE_BRACE "{}"
#endif

#ifndef DEBUG_RANGE_COMMA
#define DEBUG_RANGE_COMMA ", "
#endif

#ifndef DEBUG_TUPLE_BRACE
#define DEBUG_TUPLE_BRACE "{}"
#endif

#ifndef DEBUG_TUPLE_COMMA
#define DEBUG_TUPLE_COMMA ", "
#endif

#ifndef DEBUG_ENUM_BRACE
#define DEBUG_ENUM_BRACE "()"
#endif

#ifndef DEBUG_SHOW_NULLOPT
#define DEBUG_SHOW_NULLOPT "nullopt"
#endif

#ifndef DEBUG_UNKNOWN_TYPE_BRACE
#define DEBUG_UNKNOWN_TYPE_BRACE "[]"
#endif

#ifndef DEBUG_UNKNOWN_TYPE_AT
#define DEBUG_UNKNOWN_TYPE_AT '@'
#endif

#ifndef DEBUG_POINTER_HEXADECIMAL_PREFIX
#define DEBUG_POINTER_HEXADECIMAL_PREFIX "0x"
#endif

#ifdef DEBUG_UNSIGNED_AS_HEXADECIMAL
#define DEBUG_UNSIGNED_AS_HEXADECIMAL 0
#endif

#ifdef DEBUG_HEXADECIMAL_UPPERCASE
#define DEBUG_HEXADECIMAL_UPPERCASE 0
#endif

#ifdef DEBUG_CLASS_NAME
#define debug DEBUG_CLASS_NAME
#endif

#if DEBUG_LEVEL

#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#if DEBUG_LEVEL >= 2
#include <fstream>
#include <unordered_map>
#endif
#if defined(__has_include)
#if __has_include(<source_location>)
#include <source_location>
#define DEBUG_SOURCE_LOCATION std::source_location
#elif __has_include(<experimental/source_location>)
#include <experimental/source_location>
#define DEBUG_SOURCE_LOCATION std::experimental::source_location
#endif
#endif
#endif
#include <type_traits>
#include <typeinfo>
#include <sstream>
#include <string>
#include <memory>
#ifndef DEBUG_CUSTOM_DEMANGLE
#ifndef DEBUG_HAS_CXXABI_H
#if defined(__has_include)
#if defined(__unix__) && __has_include(<cxxabi.h>)
#include <cxxabi.h>
#define DEBUG_HAS_CXXABI_H
#endif
#endif
#else
#include <cxxabi.h>
#endif
#endif

DEBUG_NAMESPACE_BEGIN

#ifndef DEBUG_SOURCE_LOCATION
#define DEBUG_SOURCE_LOCATION std::in_place_t
#define DEBUG_SOURCE_LOCATION_CURRENT std::in_place
#else
#define DEBUG_SOURCE_LOCATION_CURRENT DEBUG_SOURCE_LOCATION::current()
#endif

#if defined(__has_include)
#if __has_include(<string_view>)
#include <string_view>
#define DEBUG_STRING_VIEW std::string_view
#elif __has_include(<experimental/string_view>)
#include <experimental/string_view>
#define DEBUG_STRING_VIEW std::experimental::string_view
#else
#include <string>
#define DEBUG_STRING_VIEW std::string
#endif

#if defined(__has_cpp_attribute)
#if __has_cpp_attribute(unlikely)
#define DEBUG_UNLIKELY [[unlikely]]
#else
#define DEBUG_UNLIKELY
#endif
#if __has_cpp_attribute(nodiscard)
#define DEBUG_NODISCARD [[nodiscard]]
#else
#define DEBUG_NODISCARD
#endif
#else
#define DEBUG_UNLIKELY
#define DEBUG_NODISCARD
#endif

#if !(__cpp_if_constexpr && __cpp_concepts && \
      __cpp_lib_type_trait_variable_templates)
template <class T>
struct debug_format_trait {
    void operator()(std::ostream &oss, T const &t) const {
        oss << t;
    }
};
#endif

struct DEBUG_NODISCARD debug {
private:
    std::ostream &cout;
    std::ostringstream oss;

    enum {
        silent = 0,
        print = 1,
        panic = 2,
        supress = 3,
    } state;

    char const *line;
    DEBUG_SOURCE_LOCATION loc;

    static void debug_quotes(std::ostream &oss, DEBUG_STRING_VIEW sv,
                             char quote) {
        oss << quote;
        for (char c: sv) {
            switch (c) {
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            case '\\': oss << "\\\\"; break;
            case '\0': oss << "\\0"; break;
            default:
                if ((c >= 0 && c < 0x20) || c == 0x7F
#if DEBUG_SUPRESS_NON_ASCII
                    || (static_cast<unsigned char>(c) >= 0x80)
#endif
                ) {
                    auto f = oss.flags();
                    oss << "\\x" << std::hex << std::setfill('0')
                        << std::setw(2) << static_cast<int>(c)
#if EBUG_HEXADECIMAL_UPPERCASE
                        << std::uppercase
#endif
                        ;
                    oss.flags(f);
                } else {
                    if (c == quote) {
                        oss << '\\';
                    }
                    oss << c;
                }
                break;
            }
        }
        oss << quote;
    }

#ifdef DEBUG_CUSTOM_DEMANGLE
    static std::string debug_demangle(char const *name) {
        return DEBUG_CUSTOM_DEMANGLE(name);
    }
#else
    static std::string debug_demangle(char const *name) {
#ifdef DEBUG_HAS_CXXABI_H
        int status;
        char *p = abi::__cxa_demangle(name, 0, 0, &status);
        std::string s = p ? p : name;
        std::free(p);
#else
        std::string s = name;
#endif
        return s;
    }
#endif

#if __cpp_if_constexpr && __cpp_concepts && \
    __cpp_lib_type_trait_variable_templates
    template <class T>
    static void debug_format(std::ostream &oss, T const &t) {
        using std::begin;
        using std::end;
        using std::to_string;
        if constexpr ((std::is_convertible_v<T, DEBUG_STRING_VIEW> ||
                       std::is_convertible_v<
                           T, std::string>)&&!std::is_same_v<T, char const *>) {
            if constexpr (!std::is_convertible_v<T, DEBUG_STRING_VIEW>) {
                std::string s = t;
                debug_quotes(oss, s, '"');
            } else {
                debug_quotes(oss, t, '"');
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            auto f = oss.flags();
            oss << std::boolalpha << t;
            oss.flags(f);
        } else if constexpr (std::is_same_v<T, char> ||
                             std::is_same_v<T, signed char>) {
            debug_quotes(oss, {reinterpret_cast<char const *>(&t), 1}, '\'');
        } else if constexpr (
#if __cpp_char8_t
            std::is_same_v<T, char8_t> ||
#endif
            std::is_same_v<T, char16_t> || std::is_same_v<T, char32_t>) {
            auto f = oss.flags();
            oss << "'\\"
                << " xu U"[sizeof(T)] << std::hex << std::setfill('0')
                << std::setw(sizeof(T) * 2)
#if DEBUG_HEXADECIMAL_UPPERCASE
                << std::uppercase
#endif
                << static_cast<std::uint32_t>(t) << "'";
            oss.flags(f);
        } else if constexpr (std::is_integral_v<T>
#if DEBUG_UNSIGNED_AS_HEXADECIMAL
                             && std::is_unsigned_v<T>
#else
                             && 0
#endif
        ) {
            auto f = oss.flags();
            oss << "0x" << std::hex << std::setfill('0')
#if DEBUG_UNSIGNED_AS_HEXADECIMAL >= 2
                << std::setw(sizeof(T) * 2)
#endif
#if DEBUG_HEXADECIMAL_UPPERCASE
                << std::uppercase
#endif
                ;
            if constexpr (sizeof(T) == 1) {
                oss << static_cast<unsigned int>(t);
            } else {
                oss << t;
            }
            oss.flags(f);
        } else if constexpr (std::is_floating_point_v<T>) {
            auto f = oss.flags();
            oss << std::fixed
                << std::setprecision(std::numeric_limits<T>::digits10) << t;
            oss.flags(f);
        } else if constexpr (requires(std::ostream &oss, T const &t) {
                                 oss << t;
                             }) {
            oss << t;
        /* } else if constexpr (requires(T const &t) { to_string(t); }) { */
        /*     oss << to_string(t); */
        } else if constexpr (std::is_pointer_v<T>) {
            auto f = oss.flags();
            oss << DEBUG_POINTER_HEXADECIMAL_PREFIX << std::hex << std::setfill('0')
#if DEBUG_HEXADECIMAL_UPPERCASE
                << std::uppercase
#endif
                ;
            oss << reinterpret_cast<std::uintptr_t>(
                reinterpret_cast<void const volatile *>(t));
            oss.flags(f);
        } else if constexpr (requires(T const &t) { begin(t) != end(t); }) {
            oss << DEBUG_RANGE_BRACE[0];
            bool add_comma = false;
            for (auto &&i: t) {
                if (add_comma) {
        oss << DEBUG_RANGE_COMMA;
    }
                add_comma = true;
                debug_format(oss, std::forward<decltype(i)>(i));
            }
            oss << DEBUG_RANGE_BRACE[1];
        } else if constexpr (requires(T const &t) {
                                 std::tuple_size<T>::value;
                             }) {
            oss << DEBUG_TUPLE_BRACE[0];
            bool add_comma = false;
            std::apply(
                [&](auto &&...args) {
                    (([&] {
                         if (add_comma) {
            oss << DEBUG_TUPLE_COMMA;
        }
                         add_comma = true;
                         debug_format(oss, std::forward<decltype(args)>(args));
                     }()),
                     ...);
                },
                t);
            oss << DEBUG_TUPLE_BRACE[1];
        } else if constexpr (std::is_enum_v<T>) {
#ifdef DEBUG_MAGIC_ENUM
            oss << DEBUG_MAGIC_ENUM(t);
#else
            oss << debug_demangle(typeid(T).name()) << DEBUG_ENUM_BRACE[0];
            oss << static_cast<std::underlying_type_t<T>>(t);
            oss << DEBUG_ENUM_BRACE[1];
#endif
        } else if constexpr (std::is_same_v<T, std::type_info>) {
            oss << debug_demangle(t.name());
        } else if constexpr (requires(T const &t) { t.DEBUG_REPR_NAME(); }) {
            debug_format(oss, t.DEBUG_REPR_NAME());
        } else if constexpr (requires(T const &t) { t.DEBUG_REPR_NAME(oss); }) {
            t.DEBUG_REPR_NAME(oss);
        } else if constexpr (requires(T const &t) { DEBUG_REPR_NAME(t); }) {
            debug_format(oss, DEBUG_REPR_NAME(t));
        } else if constexpr (requires(T const &t) { DEBUG_REPR_NAME(oss, t); }) {
            DEBUG_REPR_NAME(oss, t);
        } else if constexpr (requires(T const &t) {
                                 visit([](auto const &) {}, t);
                             }) {
            visit([&oss](auto const &t) { debug_format(oss, t); }, t);
        } else if constexpr (requires(T const &t) {
                                 (*t);
                                 (bool)t;
                             }) {
            if ((bool)t) {
                debug_format(oss, *t);
            } else {
                oss << DEBUG_SHOW_NULLOPT;
            }
        } else {
            oss << DEBUG_UNKNOWN_TYPE_BRACE[0]
                << debug_demangle(typeid(t).name()) << DEBUG_UNKNOWN_TYPE_AT;
            debug_format(oss,
                         reinterpret_cast<void const *>(std::addressof(t)));
            oss << DEBUG_UNKNOWN_TYPE_BRACE[1];
        }
    }
#else
    template <class T>
    static void debug_format(std::ostream &oss, T const &t) {
        debug_format_trait<T>()(oss, t);
    }
#endif

    debug &add_location_marks() {
        char const *fn = loc.file_name();
        for (char const *fp = fn; *fp; ++fp) {
            if (*fp == '/') {
                fn = fp + 1;
            }
        }
        oss << fn << ':' << loc.line() << ':' << DEBUG_SEPARATOR_TAB;
        if (line) {
            oss << '[' << line << ']' << DEBUG_SEPARATOR_TAB;
#if DEBUG_LEVEL >= 2
        } else {
            static thread_local std::unordered_map<std::string, std::string>
                fileCache;
            auto key = std::to_string(loc.line()) + loc.file_name();
            if (auto it = fileCache.find(key);
                it != fileCache.end() && !it->second.empty()) [[likely]] {
                oss << '[' << it->second << ']';
            } else if (auto file = std::ifstream(loc.file_name());
                       file.is_open()) [[likely]] {
                std::string line;
                for (int i = 0; i < loc.line(); ++i) {
                    if (!std::getline(file, line)) DEBUG_UNLIKELY {
                        line.clear();
                        break;
                    }
                }
                if (auto pos = line.find_first_not_of(" \t\r\n");
                    pos != line.npos) [[likely]] {
                    line = line.substr(pos);
                }
                if (!line.empty()) [[likely]] {
                    if (line.back() == ';') [[likely]] {
                        line.pop_back();
                    }
                    oss << '[' << line << ']';
                }
                fileCache.try_emplace(key, std::move(line));
            } else {
                oss << '[' << '?' << ']';
                fileCache.try_emplace(key);
            }
#endif
        }
        oss << ' ';
        return *this;
    }

    template <class T>
    struct DEBUG_NODISCARD debug_condition {
    private:
        debug &d;
        T const &t;

        template <class U>
        debug &check(bool cond, U const &u, char const *sym) {
            if (!cond) DEBUG_UNLIKELY {
                d.on_error("assertion failed:") << t << sym << u;
            }
            return d;
        }

    public:
        explicit debug_condition(debug &d, T const &t) noexcept : d(d), t(t) {}

        template <class U>
        debug &operator<(U const &u) {
            return check(t < u, u, "<");
        }

        template <class U>
        debug &operator>(U const &u) {
            return check(t > u, u, ">");
        }

        template <class U>
        debug &operator<=(U const &u) {
            return check(t <= u, u, "<=");
        }

        template <class U>
        debug &operator>=(U const &u) {
            return check(t >= u, u, ">=");
        }

        template <class U>
        debug &operator==(U const &u) {
            return check(t == u, u, "==");
        }

        template <class U>
        debug &operator!=(U const &u) {
            return check(t != u, u, "!=");
        }
    };

    debug &on_error(char const *msg) {
        if (state != supress) {
            state = panic;
            add_location_marks();
        } else {
            oss << ' ';
        }
        oss << msg;
        return *this;
    }

    template <class T>
    debug &on_print(T const &t) {
        if (state == supress)
            return *this;
        if (state == silent) {
            state = print;
            add_location_marks();
        } else {
            oss << ' ';
        }
        debug_format(oss, t);
        return *this;
    }

#if DEBUG_ENABLE_FILES_MATCH
    static bool file_detected(const char *file) noexcept {
        static auto files = std::getenv("DEBUG_FILES");
        if (!files) return true;
        DEBUG_STRING_VIEW sv = files;
        /* std::size_t pos = 0, nextpos; */
        /* while ((nextpos = sv.find(' ')) != sv.npos) { */
        /*     if (sv.substr(pos, nextpos - pos) == tag) { */
        /*         return true; */
        /*     } */
        /*     pos = nextpos + 1; */
        /* } */
        if (sv.find(file) != sv.npos) {
            return true;
        }
        return false;
    }
#endif

public:
    explicit debug(bool enable = DEBUG_DEFAULT_ENABLED,
          std::ostream &cout = DEBUG_DEFAULT_OUTPUT, char const *line = nullptr,
          DEBUG_SOURCE_LOCATION const &loc =
              DEBUG_SOURCE_LOCATION_CURRENT) noexcept
        : cout(cout),
          state(enable
#if DEBUG_ENABLE_FILES_MATCH
                && file_detected(loc.file_name())
#endif
                ? silent : supress),
          line(line),
          loc(loc) {}

    debug(debug &&) = delete;
    debug(debug const &) = delete;

    template <class T>
    debug_condition<T> check(T const &t) noexcept {
        return debug_condition<T>{*this, t};
    }

    template <class T>
    debug_condition<T> operator>>(T const &t) noexcept {
        return debug_condition<T>{*this, t};
    }

    debug &fail(bool fail = true) {
        if (fail) DEBUG_UNLIKELY {
            on_error("failed:");
        } else {
            state = supress;
        }
        return *this;
    }

    debug &on(bool enable) {
        if (!enable) [[likely]] {
            state = supress;
        }
        return *this;
    }

    template <class T>
    debug &operator<<(T const &t) {
        return on_print(t);
    }

    template <class T>
    debug &operator,(T const &t) {
        return on_print(t);
    }

    ~debug()
#if DEBUG_PANIC_METHOD == 0
noexcept(false)
#endif
{
        if (state == panic) DEBUG_UNLIKELY {
#if DEBUG_PANIC_METHOD == 0
            throw std::runtime_error(oss.str());
#elif DEBUG_PANIC_METHOD == 1
            oss << '\n';
            cout << oss.str();
#if defined(_MSC_VER)
            __debugbreak();
            return;
#elif defined(__GNUC__) && defined(__has_builtin)
#if __has_builtin(__builtin_trap)
            __builtin_trap();
            return;
#else
            std::terminate();
#endif
#else
            std::terminate();
#endif
#elif DEBUG_PANIC_METHOD == 2
            oss << '\n';
            cout << oss.str();
            std::terminate();
#else
            oss << '\n';
            cout << oss.str();
            return;
#endif
        }
        if (state == print) {
            oss << '\n';
            cout << oss.str();
        }
    }
};

DEBUG_NAMESPACE_END

#else

DEBUG_NAMESPACE_BEGIN

struct DEBUG_NODISCARD debug {
    debug(bool = true, char const * = nullptr) noexcept {}

    debug(debug &&) = delete;
    debug(debug const &) = delete;

    template <class T>
    debug &operator,(T const &) {
        return *this;
    }

    template <class T>
    debug &operator<<(T const &) {
        return *this;
    }

    debug &on(bool) {
        return *this;
    }

    debug &fail(bool = true) {
        return *this;
    }

    ~debug() noexcept(false) {}

private:
    struct DEBUG_NODISCARD debug_condition {
        debug &d;

        explicit debug_condition(debug &d) : d(d) {}

        template <class U>
        debug &operator<(U const &) {
            return d;
        }

        template <class U>
        debug &operator>(U const &) {
            return d;
        }

        template <class U>
        debug &operator<=(U const &) {
            return d;
        }

        template <class U>
        debug &operator>=(U const &) {
            return d;
        }

        template <class U>
        debug &operator==(U const &) {
            return d;
        }

        template <class U>
        debug &operator!=(U const &) {
            return d;
        }
    };

public:
    template <class T>
    debug_condition check(T const &) noexcept {
        return debug_condition{*this};
    }

    template <class T>
    debug_condition operator>>(T const &) noexcept {
        return debug_condition{*this};
    }
};

DEBUG_NAMESPACE_END

#endif

#ifdef DEBUG_CLASS_NAME
#undef debug
#endif
