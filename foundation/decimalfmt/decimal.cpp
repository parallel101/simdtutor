#include <cstring>
#include <string>
#include <vector>
#include <array>
#include <random>
#include <algorithm>
#include <fmt/format.h>
#include <immintrin.h>
#include "show_time.h"


using S = std::array<char, 9>;


void randomize_data(S *a, size_t n)
{
    std::generate(a, a + n, [rng = std::mt19937{}, uni = std::uniform_int_distribution<int32_t>{600000, 605999}] () mutable {
        S r{};
        fmt::format_to(r.data(), "{:06d}", uni(rng));
        return r;
    });
}


uint32_t atou6(const char *s)
{
    uint32_t i;
    i = 100000 * (*s++ - '0');
    i += 10000 * (*s++ - '0');
    i += 1000 * (*s++ - '0');
    i += 100 * (*s++ - '0');
    i += 10 * (*s++ - '0');
    i += *s++ - '0';
    return i;
}


uint32_t atou6q(const char *s)
{
    __m128i m = _mm_loadl_epi64((const __m128i *)s);
    m = _mm_sub_epi8(m, _mm_set1_epi8('0'));
    m = _mm_unpacklo_epi8(m, _mm_setzero_si128());
    m = _mm_mullo_epi16(m, _mm_set_epi16(0, 0, 1, 10, 100, 1000, 1, 10));
    m = _mm_shuffle_epi32(m, 0b00111001);
    m = _mm_hadd_epi16(m, m);
    m = _mm_hadd_epi16(m, m);
    m = _mm_unpacklo_epi16(m, _mm_setzero_si128());
    m = _mm_mullo_epi32(m, _mm_set_epi32(0, 0, 10000, 1));
    m = _mm_hadd_epi32(m, m);
    return _mm_cvtsi128_si32(m);

    // return m[0] & 0xffff;
    // return m[1] & 0xffff;
    // m = _mm_hadd_epi16(m, m);
    // return (uint32_t)(_mm_cvtsi128_si32(m) & 0xffff) + UINT32_C(600000);
}


uint32_t atou6qs(const char *s)
{
    __m128i m = _mm_loadl_epi64((const __m128i *)s);
    m = _mm_sub_epi8(m, _mm_set_epi8(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '0', '0', '0', '0', '0', '0'));
    m = _mm_unpacklo_epi8(m, _mm_setzero_si128());
    m = _mm_mullo_epi16(m, _mm_set_epi16(0, 0, 1, 10, 100, 1000, 0, 0x6a0));
    m = _mm_shuffle_epi32(m, 0b11001001);
    m = _mm_hadd_epi16(m, m);
    m = _mm_hadd_epi16(m, m);
    m = _mm_hadd_epi16(m, m);
    m = _mm_insert_epi16(m, 0x9, 1);
    // auto a = (uint16_t *)&m;
    // fmt::println("{}:{}:{}:{}:{}:{}:{}:{}", a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]);
    return _mm_cvtsi128_si32(m);
}


uint32_t atou6q0(const char *s)
{
    __m128i m = _mm_loadl_epi64((const __m128i *)s);
    m = _mm_sub_epi8(m, _mm_set1_epi8('0'));
    m = _mm_unpacklo_epi8(m, _mm_setzero_si128());
    m = _mm_mullo_epi16(m, _mm_set_epi16(0, 0, 1, 10, 100, 1000, 0, 0));
    m = _mm_shuffle_epi32(m, 0b00001001);
    m = _mm_hadd_epi16(m, m);
    m = _mm_hadd_epi16(m, m);
    return _mm_cvtsi128_si32(m);
}


uint32_t atou6qo(const char input[4]) {
    // Load 4 bytes into a 32-bit temporary
    uint32_t tmp;
    __builtin_memcpy(&tmp, input, 4);

    // Zero-extend bytes to 32-bit integers
    __m128i ascii = _mm_cvtepu8_epi32(_mm_cvtsi32_si128(tmp));

    __m128i digits = _mm_sub_epi32(ascii, _mm_set1_epi32('0'));

    // Set multipliers: [1000, 100, 10, 1]
    __m128i multipliers = _mm_set_epi32(1, 10, 100, 1000);

    // Multiply digits by multipliers
    __m128i products = _mm_mullo_epi32(digits, multipliers);

    // Horizontal sum: shift and add elements
    __m128i sum_a = _mm_add_epi32(products, _mm_srli_si128(products, 8));
    __m128i sum_b = _mm_add_epi32(sum_a, _mm_srli_si128(sum_a, 4));

    // Extract the result
    return _mm_cvtsi128_si32(sum_b);
}


uint32_t atou6q6(const char *s)
{
    __m128i m = _mm_loadl_epi64((const __m128i *)s);
    m = _mm_sub_epi8(m, _mm_set1_epi8('0'));
    m = _mm_unpacklo_epi8(m, _mm_setzero_si128());
    m = _mm_mullo_epi16(m, _mm_set_epi16(0, 0, 1, 10, 100, 1000, 0, 0));
    m = _mm_shuffle_epi32(m, 0b00001001);
    m = _mm_hadd_epi16(m, m);
    m = _mm_hadd_epi16(m, m);
    return _mm_cvtsi128_si32(m) + 600000;
}


void atoi_conv(S *a, int32_t *b, size_t n)
{
    for (size_t i = 0; i < n; ++i) {
        b[i] = atoi(a[i].data());
    }
}


void strtoul_conv(S *a, int32_t *b, size_t n)
{
    for (size_t i = 0; i < n; ++i) {
        b[i] = strtoul(a[i].data(), nullptr, 10);
    }
}


void atou6_conv(S *a, int32_t *b, size_t n)
{
    for (size_t i = 0; i < n; ++i) {
        b[i] = atou6(a[i].data());
    }
}


void atou6q_conv(S *a, int32_t *b, size_t n)
{
    for (size_t i = 0; i < n; ++i) {
        b[i] = atou6q(a[i].data());
    }
}


void atou6q6_conv(S *a, int32_t *b, size_t n)
{
    for (size_t i = 0; i < n; ++i) {
        b[i] = atou6q6(a[i].data());
    }
}


void atou6qs_conv(S *a, int32_t *b, size_t n)
{
    for (size_t i = 0; i < n; ++i) {
        b[i] = atou6qs(a[i].data());
    }
}


void atou6q0_conv(S *a, int32_t *b, size_t n)
{
    for (size_t i = 0; i < n; ++i) {
        b[i] = atou6q0(a[i].data());
    }
}


void atou6qo_conv(S *a, int32_t *b, size_t n)
{
    for (size_t i = 0; i < n; ++i) {
        b[i] = atou6qo(a[i].data() + 2);
    }
}


int main()
{
    std::vector<S> ini(1000000);
    randomize_data(ini.data(), ini.size());

    {
        std::vector<S> a = ini;
        std::vector<int32_t> b(a.size());
        {
            show_time _("atou6");
            atou6_conv(a.data(), b.data(), a.size());
            atou6_conv(a.data(), b.data(), a.size());
            atou6_conv(a.data(), b.data(), a.size());
            atou6_conv(a.data(), b.data(), a.size());
            atou6_conv(a.data(), b.data(), a.size());
            atou6_conv(a.data(), b.data(), a.size());
        }
    }
    
    {
        std::vector<S> a = ini;
        std::vector<int32_t> b(a.size());
        {
            show_time _("atou6q");
            atou6q_conv(a.data(), b.data(), a.size());
            atou6q_conv(a.data(), b.data(), a.size());
            atou6q_conv(a.data(), b.data(), a.size());
            atou6q_conv(a.data(), b.data(), a.size());
            atou6q_conv(a.data(), b.data(), a.size());
            atou6q_conv(a.data(), b.data(), a.size());
        }
    }

    {
        std::vector<S> a = ini;
        std::vector<int32_t> b(a.size());
        {
            show_time _("atou6qs");
            atou6qs_conv(a.data(), b.data(), a.size());
            atou6qs_conv(a.data(), b.data(), a.size());
            atou6qs_conv(a.data(), b.data(), a.size());
            atou6qs_conv(a.data(), b.data(), a.size());
            atou6qs_conv(a.data(), b.data(), a.size());
            atou6qs_conv(a.data(), b.data(), a.size());
        }
    }
    
    {
        std::vector<S> a = ini;
        std::vector<int32_t> b(a.size());
        {
            show_time _("atou6q6");
            atou6q6_conv(a.data(), b.data(), a.size());
            atou6q6_conv(a.data(), b.data(), a.size());
            atou6q6_conv(a.data(), b.data(), a.size());
            atou6q6_conv(a.data(), b.data(), a.size());
            atou6q6_conv(a.data(), b.data(), a.size());
            atou6q6_conv(a.data(), b.data(), a.size());
        }
    }
    
    {
        std::vector<S> a = ini;
        std::vector<int32_t> b(a.size());
        {
            show_time _("atou6q0");
            atou6q0_conv(a.data(), b.data(), a.size());
            atou6q0_conv(a.data(), b.data(), a.size());
            atou6q0_conv(a.data(), b.data(), a.size());
            atou6q0_conv(a.data(), b.data(), a.size());
            atou6q0_conv(a.data(), b.data(), a.size());
            atou6q0_conv(a.data(), b.data(), a.size());
        }
    }

    {
        std::vector<S> a = ini;
        std::vector<int32_t> b(a.size());
        {
            show_time _("atou6qo");
            atou6qo_conv(a.data(), b.data(), a.size());
            atou6qo_conv(a.data(), b.data(), a.size());
            atou6qo_conv(a.data(), b.data(), a.size());
            atou6qo_conv(a.data(), b.data(), a.size());
            atou6qo_conv(a.data(), b.data(), a.size());
            atou6qo_conv(a.data(), b.data(), a.size());
        }
    }
    
    {
        std::vector<S> a = ini;
        std::vector<int32_t> b(a.size());
        {
            show_time _("atoi");
            atoi_conv(a.data(), b.data(), a.size());
            atoi_conv(a.data(), b.data(), a.size());
            atoi_conv(a.data(), b.data(), a.size());
            atoi_conv(a.data(), b.data(), a.size());
            atoi_conv(a.data(), b.data(), a.size());
            atoi_conv(a.data(), b.data(), a.size());
        }
    }
    
    {
        std::vector<S> a = ini;
        std::vector<int32_t> b(a.size());
        {
            show_time _("strtoul");
            strtoul_conv(a.data(), b.data(), a.size());
            strtoul_conv(a.data(), b.data(), a.size());
            strtoul_conv(a.data(), b.data(), a.size());
            strtoul_conv(a.data(), b.data(), a.size());
            strtoul_conv(a.data(), b.data(), a.size());
            strtoul_conv(a.data(), b.data(), a.size());
        }
    }

    fmt::println("{}", atou6qo("685123\0\0\0"));
}
