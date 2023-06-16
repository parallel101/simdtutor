#if _

// 251 ns 0.12 cpi
size_t findp(float const *x, size_t n, float y) {
    __m128 pred = _mm_set1_ps(y);
    __m128i index = _mm_setr_epi32(0, 1, 2, 3);
    size_t i;
    for (i = 0; i + 4 <= n; i += 4) {
        __m128 xi = _mm_loadu_ps(x + i);
        __m128 mask = _mm_cmpgt_ps(xi, pred);
        int m = _mm_movemask_ps(mask);
        if (m) [[unlikely]] {
            return _tzcnt_u32(m) + i;
        }
    }
    for (; i < n; i++) {
        __m128 xi = _mm_load_ss(x + i);
        __m128 mask = _mm_cmpgt_ss(xi, pred);
        int m = _mm_extract_ps(mask, 0);
        if (m) {
            return i;
        }
    }
    return (size_t)-1;
}

// 749 ns 0.37 cpi
size_t findp(float const *x, size_t n, float y) {
    for (size_t i = 0; i < n; i++) {
        if (x[i] > y)
            return i;
    }
    return (size_t)-1;
}

// 180 ns 0.09 cpi
size_t findp(float const *x, size_t n, float y) {
    __m128 pred = _mm_set1_ps(y);
    __m128i index = _mm_setr_epi32(0, 1, 2, 3);
    size_t i;
    for (i = 0; i + 16 <= n; i += 16) {
        __m128 xi = _mm_loadu_ps(x + i);
        __m128 xi2 = _mm_loadu_ps(x + i + 4);
        __m128 xi3 = _mm_loadu_ps(x + i + 8);
        __m128 xi4 = _mm_loadu_ps(x + i + 12);
        xi = _mm_cmpgt_ps(xi, pred);
        xi2 = _mm_cmpgt_ps(xi2, pred);
        xi3 = _mm_cmpgt_ps(xi3, pred);
        xi4 = _mm_cmpgt_ps(xi4, pred);
        xi = _mm_or_ps(xi, xi2);
        xi3 = _mm_or_ps(xi3, xi4);
        xi = _mm_or_ps(xi, xi3);
        if (_mm_movemask_ps(xi)) [[unlikely]] {
            __m128 xi = _mm_loadu_ps(x + i);
            __m128 xi2 = _mm_loadu_ps(x + i + 4);
            __m128 xi3 = _mm_loadu_ps(x + i + 8);
            __m128 xi4 = _mm_loadu_ps(x + i + 12);
            xi = _mm_cmpgt_ps(xi, pred);
            xi2 = _mm_cmpgt_ps(xi2, pred);
            xi3 = _mm_cmpgt_ps(xi3, pred);
            xi4 = _mm_cmpgt_ps(xi4, pred);
            int m = _mm_movemask_ps(xi);
            int m2 = _mm_movemask_ps(xi2);
            int m3 = _mm_movemask_ps(xi3);
            int m4 = _mm_movemask_ps(xi4);
            m |= m2 << 4;
            m3 |= m4 << 4;
            m |= m3 << 8;
            return _tzcnt_u32(m) + i;
        }
    }
    for (; i < n; i++) {
        __m128 xi = _mm_load_ss(x + i);
        __m128 mask = _mm_cmpgt_ss(xi, pred);
        int m = _mm_extract_ps(mask, 0);
        if (m) {
            return i;
        }
    }
    return (size_t)-1;
}

#endif
