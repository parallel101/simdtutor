#include <bit>
#include <cstdint>
#include <cstring>
#include <cwctype>
#include <exception>
#include <format>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <locale>
#include <ranges>

void hexdump(std::ranges::input_range auto const &range) {
    using T = std::ranges::range_value_t<decltype(range)>;
    bool add_space = false;
    std::wstring s;
    for (auto c : range) {
        if (add_space) {
            std::wcout << ' ';
        } else {
            add_space = true;
        }
        std::wcout << std::right << std::hex << std::setw(2)
                   << std::setfill(L'0');
        std::wcout << (std::uint64_t)(std::make_unsigned_t<T>)c;
        auto w = (wchar_t)(std::make_unsigned_t<T>)c;
        if (!std::iswprint(w)) {
            if (w <= 0x20) {
                w += 0x2400;
            } else if (w == 0x7f) {
                w = 0x2421;
            } else {
                w = L'.';
            }
        }
        s.push_back(w);
    }
    std::wcout << "\t|";
    std::wcout << s;
    std::wcout << "|\n";
}

template <class Ch, class Strategy> struct UtfTrait;

template <class Strategy> struct UtfTrait<char8_t, Strategy> {
    template <std::input_iterator It>
        requires std::convertible_to<std::iter_value_t<It>, char8_t>
    static constexpr char32_t nextCodePoint(It &it,
                                            std::sentinel_for<It> auto ite) {
        char32_t c = (char8_t)*it++;
        switch (std::countl_one((std::uint8_t)c)) {
        case 0: { // 0b0xxx'xxxx -> 0x0 ~ 0x7F
            return c;
        }
        case 2: { // 0b110x'xxxx -> 0x80 ~ 0x7FF
            if (it == ite) [[unlikely]]
                return 0xFFFD;
            char32_t c2 = (char8_t)*it++;
            if ((c2 >> 6) != 0b10) [[unlikely]]
                return 0xFFFD;
            c = (c & 0b0001'1111) << 6 | c2 & 0b0011'1111;
            if (c < 0x80) [[unlikely]]
                return 0xFFFD;
            return c;
        }
        case 3: { // 0b1110'xxxx -> 0x800 ~ 0xFFFF
            if (it == ite) [[unlikely]]
                return 0xFFFD;
            char32_t c2 = (char8_t)*it++;
            if ((c2 >> 6) != 0b10) [[unlikely]]
                return 0xFFFD;
            if (it == ite) [[unlikely]]
                return 0xFFFD;
            char32_t c3 = (char8_t)*it++;
            if ((c3 >> 6) != 0b10) [[unlikely]]
                return 0xFFFD;
            c = (c & 0b0000'1111) << 12 | (c2 & 0b0011'1111) << 6 |
                c3 & 0b0011'1111;
            if (c < 0x800 || (c >= 0xD800 && c <= 0xDFFF)) [[unlikely]]
                return 0xFFFD;
            return c;
        }
        case 4: { // 0b1111'0xxx -> 0x10000 ~ 0x10FFFF
            if (it == ite) [[unlikely]]
                return 0xFFFD;
            char32_t c2 = (char8_t)*it++;
            if ((c2 >> 6) != 0b10) [[unlikely]]
                return 0xFFFD;
            if (it == ite) [[unlikely]]
                return 0xFFFD;
            char32_t c3 = (char8_t)*it++;
            if ((c3 >> 6) != 0b10) [[unlikely]]
                return 0xFFFD;
            if (it == ite) [[unlikely]]
                return 0xFFFD;
            char32_t c4 = (char8_t)*it++;
            if ((c4 >> 6) != 0b10) [[unlikely]]
                return 0xFFFD;
            c = (c & 0b0000'0111) << 18 | (c2 & 0b0011'1111) << 12 |
                (c3 & 0b0011'1111) << 6 | c4 & 0b0011'1111;
            if (c < 0x10000 || c > 0x10FFFF) [[unlikely]]
                return 0xFFFD;
            return c;
        }
        default: [[unlikely]] return 0xFFFD;
        }
    }

    template <std::output_iterator<char8_t> It>
    static constexpr void putCodePoint(It &it, char32_t c) {
        if (c < 0x80) {
            *it++ = (char8_t)c;
        } else if (c < 0x800) {
            *it++ = (char8_t)(0b1100'0000 | c >> 6);
            *it++ = (char8_t)(0b1000'0000 | c & 0b0011'1111);
        } else if (c < 0x10000) {
            if (c >= 0xD800 && c <= 0xDFFF) [[unlikely]]
                return;
            *it++ = (char8_t)(0b1110'0000 | c >> 12);
            *it++ = (char8_t)(0b1000'0000 | c >> 6 & 0b0011'1111);
            *it++ = (char8_t)(0b1000'0000 | c & 0b0011'1111);
        } else {
            if (c > 0x10FFFF) [[unlikely]]
                return;
            *it++ = (char8_t)(0b1111'0000 | c >> 18);
            *it++ = (char8_t)(0b1000'0000 | c >> 12 & 0b0011'1111);
            *it++ = (char8_t)(0b1000'0000 | c >> 6 & 0b0011'1111);
            *it++ = (char8_t)(0b1000'0000 | c & 0b0011'1111);
        }
    }
};

template <class Strategy> struct UtfTrait<char16_t, Strategy> {
    template <std::input_iterator It>
        requires std::convertible_to<std::iter_value_t<It>, char16_t>
    static constexpr char32_t nextCodePoint(It &it,
                                            std::sentinel_for<It> auto ite) {
        char32_t c = (char16_t)*it++;
        if (c >= 0xDC00 && c <= 0xDFFF) [[unlikely]]
            return 0xFFFD;
        if (c >= 0xD800 && c <= 0xDBFF) {
            if (it == ite) [[unlikely]]
                return 0xFFFD;
            char32_t c2 = (char16_t)*it++;
            if (c < 0xDC00 || c2 > 0xDFFF) [[unlikely]]
                return 0xFFFD;
            c = (c & 0x3FF) << 10 | c2 & 0x3FF;
            c += 0x10000;
        }
        return c;
    }

    template <std::output_iterator<char16_t> It>
    static constexpr void putCodePoint(It &it, char32_t c) {
        if (c < 0x10000) {
            if (c >= 0xD800 && c <= 0xDFFF) [[unlikely]]
                return;
            *it++ = (char16_t)c;
        } else {
            if (c > 0x10FFFF) [[unlikely]]
                return;
            c -= 0x10000;
            *it++ = (char16_t)(0xD800 | c >> 10);
            *it++ = (char16_t)(0xDC00 | c & 0x3FF);
        }
    }
};

template <class Strategy> struct UtfTrait<char32_t, Strategy> {
    template <std::input_iterator It>
        requires std::convertible_to<std::iter_value_t<It>, char32_t>
    static constexpr char32_t nextCodePoint(It &it,
                                            std::sentinel_for<It> auto ite) {
        char32_t c = (char32_t)*it++;
        return c;
    }

    template <std::output_iterator<char32_t> It>
    static constexpr void putCodePoint(It &it, char32_t c) {
        *it++ = c;
    }
};

#ifdef _WIN32
template <class Strategy>
struct UtfTrait<wchar_t, Strategy> : UtfTrait<char16_t, Strategy> {};
#else
template <class Strategy>
struct UtfTrait<wchar_t, Strategy> : UtfTrait<char32_t, Strategy> {};
#endif

template <class Strategy>
struct UtfTrait<char, Strategy> : UtfTrait<char8_t, Strategy> {};

template <class Char, std::input_iterator It,
          std::sentinel_for<It> Sentinel = It, class Strategy = void>
    requires std::convertible_to<std::iter_value_t<It>, Char>
struct UtfInputIterator {
    using iterator_category = std::input_iterator_tag;
    using value_type = char32_t;
    using difference_type = std::iter_difference_t<It>;
    using pointer = void;
    using reference = char32_t;
    using traits_type = UtfTrait<std::iter_value_t<It>, Strategy>;

    UtfInputIterator() = default;

    constexpr explicit UtfInputIterator(It it, Sentinel ite)
        : it(it), ite(ite) {
    }

    constexpr bool operator!=(UtfInputIterator const &that) const {
        return it != that.it;
    }

    constexpr bool operator==(UtfInputIterator const &that) const {
        return it == that.it;
    }

    constexpr bool operator!=(std::default_sentinel_t) const {
        return it != ite;
    }

    constexpr bool operator==(std::default_sentinel_t) const {
        return it == ite;
    }

    friend constexpr bool operator!=(std::default_sentinel_t,
                                     UtfInputIterator const &that) {
        return that.it != that.ite;
    }

    friend constexpr bool operator==(std::default_sentinel_t,
                                     UtfInputIterator const &that) {
        return that.it == that.ite;
    }

    constexpr char32_t operator*() const {
        if (cached != (char32_t)-1)
            return cached;
        return cached = traits_type::nextCodePoint(it, ite);
    }

    constexpr UtfInputIterator &operator++() {
        if (cached == (char32_t)-1)
            cached = traits_type::nextCodePoint(it, ite);
        else
            cached = (char32_t)-1;
        return *this;
    }

    constexpr UtfInputIterator operator++(int) {
        auto tmp = *this;
        if (cached == (char32_t)-1)
            tmp.cached = cached = traits_type::nextCodePoint(it, ite);
        else
            cached = (char32_t)-1;
        return tmp;
    }

    constexpr UtfInputIterator begin() const {
        return *this;
    }

    constexpr UtfInputIterator end() const {
        return UtfInputIterator{ite, ite};
    }

    constexpr It base() const {
        return it;
    }

  private:
    mutable It it;
    Sentinel ite;
    mutable char32_t cached = (char32_t)-1;
};

template <std::input_iterator It, std::sentinel_for<It> Sentinel>
UtfInputIterator(It, Sentinel)
    -> UtfInputIterator<std::iter_value_t<It>, It, Sentinel>;

template <class Char, std::output_iterator<Char> It, class Strategy = void>
struct UtfOutputIterator {
    using iterator_category = std::output_iterator_tag;
    using value_type = Char;
    using difference_type = std::iter_difference_t<It>;
    using pointer = void;
    using reference = UtfOutputIterator;
    using traits_type = UtfTrait<Char, Strategy>;

    UtfOutputIterator() = default;

    constexpr explicit UtfOutputIterator(It it) : it(it) {
    }

    constexpr bool operator!=(UtfOutputIterator const &that) const {
        return it != that.it;
    }

    constexpr bool operator==(UtfOutputIterator const &that) const {
        return it == that.it;
    }

    constexpr UtfOutputIterator &operator=(char32_t c) {
        traits_type::putCodePoint(it, c);
        return *this;
    }

    constexpr UtfOutputIterator &
    operator=(std::ranges::input_range auto &&range)
        requires(std::convertible_to<
                 std::ranges::range_value_t<decltype(range)>, char32_t>)
    {
        for (char32_t c : range) {
            traits_type::putCodePoint(it, c);
        }
        return *this;
    }

    constexpr UtfOutputIterator &operator*() {
        return *this;
    }

    constexpr UtfOutputIterator &operator++() {
        return *this;
    }

    constexpr UtfOutputIterator &operator++(int) {
        return *this;
    }

    constexpr It base() const {
        return it;
    }

  private:
    It it;
};

template <std::output_iterator<char8_t> It>
UtfOutputIterator(It) -> UtfOutputIterator<char8_t, It>;

template <class Container>
UtfOutputIterator(std::back_insert_iterator<Container>)
    -> UtfOutputIterator<typename Container::value_type,
                         std::back_insert_iterator<Container>>;

template <class Ch>
UtfOutputIterator(std::ostreambuf_iterator<Ch>)
    -> UtfOutputIterator<Ch, std::ostreambuf_iterator<Ch>>;

template <class Ch, class Traits>
auto ostreamUtfIterator(std::basic_ostream<Ch, Traits> &os) {
    std::ostreambuf_iterator<Ch> oit(os);
    return UtfOutputIterator<Ch, std::ostreambuf_iterator<Ch>>(oit);
}

template <class Ch, class Traits>
auto istreamUtfIterator(std::basic_istream<Ch, Traits> &is) {
    std::istreambuf_iterator<Ch> iit(is), iite;
    return UtfInputIterator<Ch, std::istreambuf_iterator<Ch>>(iit, iite);
}

template <class To, class Ch, class Traits, class Alloc>
constexpr std::basic_string<
    To, std::char_traits<To>,
    typename std::allocator_traits<Alloc>::template rebind_alloc<To>>
utfConvert(std::basic_string<Ch, Traits, Alloc> const &in) {
    std::basic_string<
        To, std::char_traits<To>,
        typename std::allocator_traits<Alloc>::template rebind_alloc<To>>
        out(in.get_allocator());
    out.reserve(in.size());
    auto oit = UtfOutputIterator(std::back_inserter(out));
    auto iit = UtfInputIterator(in.begin(), in.end());
    std::copy(iit.begin(), iit.end(), oit);
    return out;
}

int main() {
    using namespace std::literals;
    std::locale::global(std::locale(""));
    std::ios::sync_with_stdio(false);
    std::ifstream fin("test.txt");
    hexdump(istreamUtfIterator(fin));
    flush(std::wcout);
    std::format_to(ostreamUtfIterator(std::cout), L"{}", utfConvert<wchar_t>("Hi, 小彭老师"s));
    flush(std::cout);
    return 0;
}
