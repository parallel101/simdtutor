#if _

// 1042 ns 0.51 cpi
size_t countp(float const *x, size_t n, float y) {
    size_t ret = 0;
    __m128 pred = _mm_set1_ps(y);
    size_t i;
    for (i = 0; i + 4 <= n; i += 4) {
        __m128 xi = _mm_loadu_ps(x + i);
        __m128 mask = _mm_cmpgt_ps(xi, pred);
        int m = _mm_movemask_ps(mask);
        ret += _mm_popcnt_u32(m);
    }
    for (; i < n; i++) {
        __m128 xi = _mm_load_ss(x + i);
        __m128 mask = _mm_cmpgt_ss(xi, pred);
        int m = _mm_extract_ps(mask, 0);
        ret += !!m;
    }
    return ret;
}

// 880 ns 0.43 cpi
size_t countp(float const *x, size_t n, float y) {
    size_t ret = 0;
    __m128 pred = _mm_set1_ps(y);
    size_t i;
    for (i = 0; i + 8 <= n; i += 8) {
        __m128 xi = _mm_loadu_ps(x + i);
        __m128 xi2 = _mm_loadu_ps(x + i + 4);
        __m128 mask = _mm_cmpgt_ps(xi, pred);
        __m128 mask2 = _mm_cmpgt_ps(xi2, pred);
        int m = _mm_movemask_ps(mask);
        int m2 = _mm_movemask_ps(mask2);
        ret += _mm_popcnt_u32(m | m2 << 4);
    }
    for (; i < n; i++) {
        __m128 xi = _mm_load_ss(x + i);
        __m128 mask = _mm_cmpgt_ss(xi, pred);
        int m = _mm_extract_ps(mask, 0);
        ret += !!m;
    }
    return ret;
}

// 1557 ns 0.76 cpi
size_t countp(float const *x, size_t n, float y) {
    size_t ret = 0;
    #pragma omp simd reduction(+:ret)
    for (size_t i = 0; i < n; i++) {
        ret += x[i] > y ? 1 : 0;
    }
    return ret;
}

// 826 ns 0.4 cpi
size_t countp(float const *x, size_t n, float y) {
    uint32_t ret = 0;
    for (size_t i = 0; i < n; i++) {
        ret += x[i] > y ? 1 : 0;
    }
    return ret;
}

// 826 ns 0.4 cpi
size_t countp(float const *x, size_t n, float y) {
    uint32_t ret = 0;
#ifndef __clang__
    #pragma omp simd reduction(+:ret)
#endif
    for (size_t i = 0; i < n; i++) {
        ret += x[i] > y ? 1 : 0;
    }
    return ret;
}

// 808 ns 0.39 cpi
size_t countp(float const *x, size_t n, float y) {
    __m128 pred = _mm_set1_ps(y);
    __m128i ret = _mm_setzero_si128();
    size_t i;
    for (i = 0; i + 8 <= n; i += 8) {
        __m128 xi = _mm_loadu_ps(x + i);
        __m128 xi2 = _mm_loadu_ps(x + i + 4);
        __m128i mask = _mm_castps_si128(_mm_cmpgt_ps(xi, pred));
        __m128i mask2 = _mm_castps_si128(_mm_cmpgt_ps(xi2, pred));
        ret = _mm_sub_epi32(ret, _mm_add_epi32(mask, mask2));
    }
    for (; i < n; i++) {
        __m128 xi = _mm_load_ss(x + i);
        __m128 mask = _mm_cmpgt_ss(xi, pred);
        int m = _mm_extract_ps(mask, 0);
        ret += !!m;
    }
    ret = _mm_add_epi32(ret, _mm_shuffle_epi32(ret, 0b01001110));
    return _mm_extract_epi32(ret, 0) + _mm_extract_epi32(ret, 1);
}

// 879 ns 0.43 cpi
size_t countp(float const *x, size_t n, float y) {
    __m128 pred = _mm_set1_ps(y);
    __m128i ret = _mm_setzero_si128();
    __m128i ret2 = _mm_setzero_si128();
    size_t i;
    for (i = 0; i + 8 <= n; i += 8) {
        __m128 xi = _mm_loadu_ps(x + i);
        __m128 xi2 = _mm_loadu_ps(x + i + 4);
        __m128i mask = _mm_castps_si128(_mm_cmpgt_ps(xi, pred));
        __m128i mask2 = _mm_castps_si128(_mm_cmpgt_ps(xi2, pred));
        ret = _mm_sub_epi32(ret, mask);
        ret2 = _mm_sub_epi32(ret2, mask2);
    }
    for (; i < n; i++) {
        __m128 xi = _mm_load_ss(x + i);
        __m128 mask = _mm_cmpgt_ss(xi, pred);
        int m = _mm_extract_ps(mask, 0);
        ret += !!m;
    }
    ret = _mm_add_epi32(ret, ret2);
    ret = _mm_add_epi32(ret, _mm_shuffle_epi32(ret, 0b01001110));
    return _mm_extract_epi32(ret, 0) + _mm_extract_epi32(ret, 1);
}

// 810 ns 0.4 cpi
size_t countp(float const *x, size_t n, float y) {
    __m128 pred = _mm_set1_ps(y);
    __m128i ret = _mm_setzero_si128();
    size_t i;
    for (i = 0; i + 8 <= n; i += 8) {
        __m128 xi = _mm_loadu_ps(x + i);
        __m128i mask = _mm_castps_si128(_mm_cmpgt_ps(xi, pred));
        __m128 xi2 = _mm_loadu_ps(x + i + 4);
        __m128i mask2 = _mm_castps_si128(_mm_cmpgt_ps(xi2, pred));
        ret = _mm_sub_epi32(ret, _mm_add_epi32(mask, mask2));
    }
    for (; i < n; i++) {
        __m128 xi = _mm_load_ss(x + i);
        __m128 mask = _mm_cmpgt_ss(xi, pred);
        int m = _mm_extract_ps(mask, 0);
        ret += !!m;
    }
    ret = _mm_add_epi32(ret, _mm_shuffle_epi32(ret, 0b01001110));
    return _mm_extract_epi32(ret, 0) + _mm_extract_epi32(ret, 1);
}

#endif
