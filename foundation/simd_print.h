#pragma once

#include "print.h"
#include <immintrin.h>

static void printhex(__m128i m) {
    printnl("{ ", as_hex(_mm_cvtsi128_si32(m)),
        ", ", as_hex(_mm_cvtsi128_si32(_mm_shuffle_epi32(m, 0x55))),
        ", ", as_hex(_mm_cvtsi128_si32(_mm_shuffle_epi32(m, 0xaa))),
        ", ", as_hex(_mm_cvtsi128_si32(_mm_shuffle_epi32(m, 0xff))),
    " }\n");
}

static void printhex16(__m128i m) {
    printnl("{ ", as_hex(_mm_cvtsi128_si32(m) & 0xff),
        ", ", as_hex(_mm_cvtsi128_si32(m) >> 16),
        ", ", as_hex(_mm_cvtsi128_si32(_mm_shuffle_epi32(m, 0x55)) & 0xff),
        ", ", as_hex(_mm_cvtsi128_si32(_mm_shuffle_epi32(m, 0x55)) >> 16),
        ", ", as_hex(_mm_cvtsi128_si32(_mm_shuffle_epi32(m, 0xaa)) & 0xff),
        ", ", as_hex(_mm_cvtsi128_si32(_mm_shuffle_epi32(m, 0xaa)) >> 16),
        ", ", as_hex(_mm_cvtsi128_si32(_mm_shuffle_epi32(m, 0xff)) & 0xff),
        ", ", as_hex(_mm_cvtsi128_si32(_mm_shuffle_epi32(m, 0xff)) >> 16),
    " }\n");
}

static void printhex(__m128d m) {
    printhex(_mm_castpd_si128(m));
}

static void printhex(__m128 m) {
    printhex(_mm_castps_si128(m));
}

static void print16(__m128i m) {
    printnl("{ ", _mm_cvtsi128_si32(m) & 0xff,
        ", ", _mm_cvtsi128_si32(m) >> 16,
        ", ", _mm_cvtsi128_si32(_mm_shuffle_epi32(m, 0x55)) & 0xff,
        ", ", _mm_cvtsi128_si32(_mm_shuffle_epi32(m, 0x55)) >> 16,
        ", ", _mm_cvtsi128_si32(_mm_shuffle_epi32(m, 0xaa)) & 0xff,
        ", ", _mm_cvtsi128_si32(_mm_shuffle_epi32(m, 0xaa)) >> 16,
        ", ", _mm_cvtsi128_si32(_mm_shuffle_epi32(m, 0xff)) & 0xff,
        ", ", _mm_cvtsi128_si32(_mm_shuffle_epi32(m, 0xff)) >> 16,
    " }\n");
}

static void print(__m128i m) {
    printnl("{ ", _mm_cvtsi128_si32(m),
        ", ", _mm_cvtsi128_si32(_mm_shuffle_epi32(m, 0x55)),
        ", ", _mm_cvtsi128_si32(_mm_shuffle_epi32(m, 0xaa)),
        ", ", _mm_cvtsi128_si32(_mm_shuffle_epi32(m, 0xff)),
    " }\n");
}

static void print(__m128d m) {
    printnl("{ ", _mm_cvtsd_f64(m),
        ", ", _mm_cvtsd_f64(_mm_shuffle_pd(m, m, 1)),
    " }\n");
}

static void print(__m128 m) {
    printnl("{ ", _mm_cvtss_f32(m),
        ", ", _mm_cvtss_f32(_mm_shuffle_ps(m, m, 1)),
        ", ", _mm_cvtss_f32(_mm_shuffle_ps(m, m, 2)),
        ", ", _mm_cvtss_f32(_mm_shuffle_ps(m, m, 3)),
    " }\n");
}

#ifdef __AVX2__

static void printhex(__m256i m) {
    printnl("{ ", as_hex(_mm256_cvtsi256_si32(m)),
        ", ", as_hex(_mm256_extract_epi32(m, 1)),
        ", ", as_hex(_mm256_extract_epi32(m, 2)),
        ", ", as_hex(_mm256_extract_epi32(m, 3)),
        ", ", as_hex(_mm256_extract_epi32(m, 4)),
        ", ", as_hex(_mm256_extract_epi32(m, 5)),
        ", ", as_hex(_mm256_extract_epi32(m, 6)),
        ", ", as_hex(_mm256_extract_epi32(m, 7)),
    " }\n");
}

static void printhex(__m256d m) {
    printhex(_mm256_castpd_si256(m));
}

static void printhex(__m256 m) {
    printhex(_mm256_castps_si256(m));
}

static void print(__m256i m) {
    printnl("{ ", _mm256_cvtsi256_si32(m),
        ", ", _mm256_extract_epi32(m, 1),
        ", ", _mm256_extract_epi32(m, 2),
        ", ", _mm256_extract_epi32(m, 3),
        ", ", _mm256_extract_epi32(m, 4),
        ", ", _mm256_extract_epi32(m, 5),
        ", ", _mm256_extract_epi32(m, 6),
        ", ", _mm256_extract_epi32(m, 7),
    " }\n");
}

static void print(__m256d m) {
    printnl("{ ", _mm256_cvtsd_f64(m),
        ", ", _mm256_cvtsd_f64(_mm256_shuffle_pd(m, m, 1)),
        ", ", _mm_cvtsd_f64(_mm256_extractf128_pd(m, 1)),
        ", ", _mm_cvtsd_f64(_mm256_extractf128_pd(_mm256_shuffle_pd(m, m, 1), 1)),
    " }\n");
}

static void print(__m256 m) {
    printnl("{ ", _mm256_cvtss_f32(m),
        ", ", _mm256_cvtss_f32(_mm256_shuffle_ps(m, m, 1)),
        ", ", _mm256_cvtss_f32(_mm256_shuffle_ps(m, m, 2)),
        ", ", _mm256_cvtss_f32(_mm256_shuffle_ps(m, m, 3)),
        ", ", _mm_cvtss_f32(_mm256_extractf128_ps(m, 1)),
        ", ", _mm_cvtss_f32(_mm256_extractf128_ps(_mm256_shuffle_ps(m, m, 1), 1)),
        ", ", _mm_cvtss_f32(_mm256_extractf128_ps(_mm256_shuffle_ps(m, m, 2), 1)),
        ", ", _mm_cvtss_f32(_mm256_extractf128_ps(_mm256_shuffle_ps(m, m, 3), 1)),
    " }\n");
}

#endif
