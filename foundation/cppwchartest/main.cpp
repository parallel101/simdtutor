#include <cstdint>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <string>
#include <iostream>
#include <locale>

template <class Ch>
std::string as_hex(std::basic_string<Ch> s) {
    std::ostringstream os;
    os << std::uppercase << std::hex;
    bool has = false;
    for (Ch c : s) {
        if (!has) {
            has = true;
        } else {
            os << ' ';
        }
        os << std::setw(sizeof(Ch) * 2) << std::setfill('0');
        os << static_cast<std::uint32_t>(static_cast<std::make_unsigned_t<Ch>>(c));
    }
    return os.str();
}

template <class WideCh, class NarrowCh>
std::basic_string<WideCh> widen(std::basic_string_view<NarrowCh> in, std::locale const &locale, WideCh replacement = sizeof(WideCh) > 1 ? WideCh(0xfffd) : WideCh('?')) {
    auto const &facet = std::use_facet<std::codecvt<WideCh, NarrowCh, std::mbstate_t>>(locale);
    std::mbstate_t mbs{};
    std::basic_string<WideCh> out;
    if constexpr (sizeof(WideCh) == sizeof(NarrowCh)) {
        if (facet.always_noconv()) [[likely]] {
            out.resize(in.size(), WideCh(0));
            std::memcpy(out.data(), in.data(), in.size() * sizeof(NarrowCh));
            return out;
        }
    }
    out.resize(in.size(), WideCh(0));
    const NarrowCh *from_next = in.data(), *from_end = in.data() + in.size();
    WideCh *to_next = out.data(), *to_end = out.data() + out.size();
    while (from_next != from_end) {
        auto result = facet.in(
            mbs, from_next, from_end, from_next,
            to_next, to_end, to_next);
        if (result != std::codecvt_base::error) [[likely]] {
            break;
        } else {
            ++from_next;
            *to_next++ = replacement;
        }
    }
    out.resize(to_next - out.data());
    return out;
}

template <class WideCh, class NarrowCh>
std::basic_string<NarrowCh> narrowing(std::basic_string_view<WideCh> in, std::locale const &locale) {
    auto const &facet = std::use_facet<std::codecvt<WideCh, NarrowCh, std::mbstate_t>>(locale);
    std::mbstate_t mbs{};
    std::basic_string<NarrowCh> out;
    if constexpr (sizeof(WideCh) == sizeof(NarrowCh)) {
        if (facet.always_noconv()) [[likely]] {
            out.resize(in.size(), NarrowCh(0));
            std::memcpy(out.data(), in.data(), in.size() * sizeof(WideCh));
            return out;
        }
    }
    int encoding = facet.encoding();
    if (encoding <= 0) [[likely]] {
        encoding = facet.max_length();
    }
    out.resize(in.size() * encoding, NarrowCh(0));
    const WideCh *from_next = in.data(), *from_end = in.data() + in.size();
    NarrowCh *to_next = out.data(), *to_end = out.data() + out.size();
    while (from_next != from_end) {
        auto result = facet.out(
            mbs, from_next, from_end, from_next,
            to_next, to_end, to_next);
        if (result != std::codecvt_base::error) [[likely]] {
            break;
        } else {
            ++from_next;
        }
    }
    out.resize(to_next - out.data());
    return out;
}

template <class To, class From>
std::basic_string<To> reinterpret_string(std::basic_string<From> const &in) {
    static_assert(sizeof(To) == sizeof(From));
    return std::basic_string<To>{reinterpret_cast<const To *>(in.data()), in.size()};
}

template <class To, class From>
std::basic_string_view<To> reinterpret_string(std::basic_string_view<From> in) {
    static_assert(sizeof(To) == sizeof(From));
    return std::basic_string_view<To>{reinterpret_cast<const To *>(in.data()), in.size()};
}

template <class To, class From>
const To *reinterpret_string(const From *in) {
    static_assert(sizeof(To) == sizeof(From));
    return reinterpret_cast<const To *>(in);
}

inline std::wstring utf8_to_wide(std::u8string_view in) {
    if constexpr (sizeof(wchar_t) == 1) {
        return std::wstring{reinterpret_string<wchar_t>(in)};
    } else if constexpr (sizeof(wchar_t) == 2) {
        return reinterpret_string<wchar_t>(widen<char16_t, char8_t>(in, std::locale::classic()));
    } else if constexpr (sizeof(wchar_t) == 4) {
        return reinterpret_string<wchar_t>(widen<char32_t, char8_t>(in, std::locale::classic()));
    }
}

inline std::u8string wide_to_utf8(std::wstring_view in) {
    if constexpr (sizeof(wchar_t) == 1) {
        return std::u8string{reinterpret_string<char8_t>(in)};
    } else if constexpr (sizeof(wchar_t) == 2) {
        return narrowing<char16_t, char8_t>(reinterpret_string<char16_t>(in), std::locale::classic());
    } else if constexpr (sizeof(wchar_t) == 4) {
        return narrowing<char32_t, char8_t>(reinterpret_string<char32_t>(in), std::locale::classic());
    }
}

inline std::string utf8_to_ansi(std::u8string_view in, std::locale const &locale = std::locale("")) {
    return narrowing<wchar_t, char>(utf8_to_wide(in), locale);
}

inline std::u8string ansi_to_utf8(std::string_view in, std::locale const &locale = std::locale("")) {
    return wide_to_utf8(widen<wchar_t, char>(in, locale));
}

inline std::string wide_to_ansi(std::wstring_view in, std::locale const &locale = std::locale("")) {
    return narrowing<wchar_t, char>(in, locale);
}

inline std::wstring ansi_to_wide(std::string_view in, std::locale const &locale = std::locale("")) {
    return widen<wchar_t, char>(in, locale);
}

inline std::u8string utf32_to_utf8(std::u32string_view in) {
    return narrowing<char32_t, char8_t>(in, std::locale::classic());
}

inline std::u32string utf8_to_utf32(std::u8string_view in) {
    return widen<char32_t, char8_t>(in, std::locale::classic());
}

inline std::u8string utf16_to_utf8(std::u16string_view in) {
    return narrowing<char16_t, char8_t>(in, std::locale::classic());
}

inline std::u16string utf8_to_utf16(std::u8string_view in) {
    return widen<char16_t, char8_t>(in, std::locale::classic());
}

#define ansi_literial(s) s
#define unicode_literial(s) L##s
#define utf8_literial(s) u8##s
#define utf16_literial(s) u##s
#define utf32_literial(s) U##s

int main() {
    using namespace std;
    auto s_ansi = "我🤔"s;
    cout << "ansi:    " << as_hex(s_ansi) << endl;
    auto s_unicode = L"我🤔"s;
    cout << "unicode: " << as_hex(s_unicode) << endl;
    auto s_utf8 = u8"我🤔"s;
    cout << "utf8:    " << as_hex(s_utf8) << endl;
    auto s_utf16 = u"我🤔"s;
    cout << "utf16:   " << as_hex(s_utf16) << endl;
    auto s_utf32 = U"我🤔"s;
    cout << "utf32:   " << as_hex(s_utf32) << endl;
    auto s_gb18030 = narrowing<wchar_t, char>(widen<wchar_t, char>("我🤔"s, std::locale("")), std::locale("zh_CN.GB18030"));
    cout << "gb18030: " << as_hex(s_gb18030) << endl;
    auto s_test = ansi_to_wide("我🤔"s, std::locale(""));
    cout << "test:    " << as_hex(s_test) << endl;
    auto s_test2 = wide_to_ansi(L"我🤔"s, std::locale("zh_CN.GBK"));
    cout << "test2:   " << as_hex(s_test2) << endl;
    auto s_test3 = wide_to_ansi(L"我🤔"s, std::locale("zh_CN.GB18030"));
    cout << "test3:   " << as_hex(s_test3) << endl;
    return 0;
}
