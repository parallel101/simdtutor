#if _

// 89939 ns 43.88 cpi
void fillsin(float *x, size_t n) {
    for (size_t i = 0; i < n; i++) {
        x[i] = sin(i);
    }
}

// 53622 ns 26.17 cpi
void fillsin(float *x, size_t n) {
    for (size_t i = 0; i < n; i++) {
        x[i] = sinf(i);
    }
}

// 29726 ns 14.51 cpi
void fillsin(float *x, size_t n) {
    for (size_t i = 0; i < n; i++) {
        __m128 ii = _mm_cvtsi32_ss(_mm_setzero_ps(), i);
        __m128 cosi;
        __m128 sini = mm_sincos_ps(ii, &cosi);
        _mm_store_ss(&x[i], sini);
    }
}

// 7266 ns 3.55 cpi
void fillsin(float *x, size_t n) {
    __m128i idx = _mm_setr_epi32(0, 1, 2, 3);
    for (size_t i = 0; i < n; i += 4) {
        __m128 ii = _mm_cvtepi32_ps(idx);
        __m128 cosi;
        __m128 sini = mm_sincos_ps(ii, &cosi);
        _mm_store_ps(&x[i], sini);
        idx = _mm_add_epi32(idx, _mm_set1_epi32(4));
    }
}

// 3615 ns 1.76 cpi
void fillsin(float *x, size_t n) {
    __m256i idx = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);
    for (size_t i = 0; i < n; i += 8) {
        __m256 ii = _mm256_cvtepi32_ps(idx);
        __m256 cosi;
        __m256 sini = mm256_sincos_ps(ii, &cosi);
        _mm256_store_ps(&x[i], sini);
        idx = _mm256_add_epi32(idx, _mm256_set1_epi32(8));
    }
}

#endif
