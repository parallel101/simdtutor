#if _

// 7741 ns 3.78 cpi
float sum(float const *x, size_t n) {
    float ret = 0.0f;
    for (size_t i = 0; i < n; i++) {
        ret += x[i];
    }
    return ret;
}

// 1933 ns 0.94 cpi
float sum(float const *x, size_t n) {
    __m128 ret = _mm_setzero_ps();
    size_t i;
    for (i = 0; i + 4 <= n; i += 4) {
        ret = _mm_add_ps(ret, _mm_loadu_ps(x + i));
    }
    ret = _mm_hadd_ps(ret, ret);
    ret = _mm_hadd_ps(ret, ret);
    for (; i < n; i++) {
        ret = _mm_add_ss(ret, _mm_load_ss(x + i));
    }
    return _mm_cvtss_f32(ret);
}

// 1937 ns 0.95 cpi
float sum(float const *x, size_t n) {
    __m128 ret = _mm_setzero_ps();
    size_t i;
    for (i = 0; i + 8 <= n; i += 8) {
        ret = _mm_add_ps(ret, _mm_loadu_ps(x + i));
        ret = _mm_add_ps(ret, _mm_loadu_ps(x + i + 4));
    }
    ret = _mm_hadd_ps(ret, ret);
    ret = _mm_hadd_ps(ret, ret);
    for (; i < n; i++) {
        ret = _mm_add_ss(ret, _mm_load_ss(x + i));
    }
    return _mm_cvtss_f32(ret);
}

// 983 ns 0.48 cpi
float sum(float const *x, size_t n) {
    __m128 ret = _mm_setzero_ps();
    __m128 ret2 = _mm_setzero_ps();
    size_t i;
    for (i = 0; i + 8 <= n; i += 8) {
        ret = _mm_add_ps(ret, _mm_loadu_ps(x + i));
        ret2 = _mm_add_ps(ret2, _mm_loadu_ps(x + i + 4));
    }
    ret = _mm_add_ps(ret, ret2);
    ret = _mm_hadd_ps(ret, ret);
    ret = _mm_hadd_ps(ret, ret);
    for (; i < n; i++) {
        ret = _mm_add_ss(ret, _mm_load_ss(x + i));
    }
    return _mm_cvtss_f32(ret);
}

// 2015 ns 0.98 cpi
float sum(float const *x, size_t n) {
    float ret = 0.0f;
    #pragma omp simd reduction(+:ret)
    for (size_t i = 0; i < n; i++) {
        ret += x[i];
    }
    return ret;
}

#endif
