#if _

// 4396 ns 2.14 cpi
[[gnu::optimize("O2")]] void saxpy(float a, float const *x, float const *y, float *z, size_t n) {
    for (size_t i = 0; i < n; i++) {
        z[i] = a * x[i] + y[i];
    }
}

// 1466 ns 0.72 cpi
void saxpy(float a, float const *x, float const *y, float *z, size_t n) {
    for (size_t i = 0; i < n; i++) {
        z[i] = a * x[i] + y[i];
    }
}

// 1488 ns 0.73 cpi
void saxpy(float a, float const *__restrict x, float const *__restrict y, float *__restrict z, size_t n) {
    for (size_t i = 0; i < n; i++) {
        z[i] = a * x[i] + y[i];
    }
}

// 1527 ns 0.75 cpi
[[gnu::optimize("O2")]] void saxpy(float a, float const *x, float const *y, float *z, size_t n) {
    for (size_t i = 0; i < n; i += 4) {
        __m128 xi = _mm_setr_ps(x[i], x[i + 1], x[i + 2], x[i + 3]);
        __m128 yi = _mm_setr_ps(y[i], y[i + 1], y[i + 2], y[i + 3]);
        __m128 zi = a * xi + yi;
        z[i] = zi[0];
        z[i + 1] = zi[1];
        z[i + 2] = zi[2];
        z[i + 3] = zi[3];
    }
}

// 1511 ns 0.74 cpi
[[gnu::optimize("O2")]] void saxpy(float a, float const *x, float const *y, float *z, size_t n) {
    for (size_t i = 0; i < n; i += 4) {
        __m128 xi = _mm_loadu_ps(&x[i]);
        __m128 yi = _mm_loadu_ps(&y[i]);
        __m128 zi = a * xi + yi;
        _mm_storeu_ps(&z[i], zi);
    }
}

// 1520 ns 0.74 cpi
[[gnu::optimize("O2")]] void saxpy(float a, float const *x, float const *y, float *z, size_t n) {
    for (size_t i = 0; i < n; i += 4) {
        __m128 xi = _mm_load_ps(&x[i]);
        __m128 yi = _mm_load_ps(&y[i]);
        __m128 zi = a * xi + yi;
        _mm_store_ps(&z[i], zi);
    }
}

// 1524 ns 0.74 cpi
[[gnu::optimize("O2")]] void saxpy(float a, float const *x, float const *y, float *z, size_t n) {
    for (size_t i = 0; i < n; i += 4) {
        __m128 xi = _mm_loadu_ps(&x[i]);
        __m128 yi = _mm_loadu_ps(&y[i]);
        __m128 zi = _mm_add_ps(_mm_mul_ps(_mm_set1_ps(a), xi), yi);
        _mm_storeu_ps(&z[i], zi);
    }
}

// 1523 ns 0.74 cpi
[[gnu::optimize("O2")]] void saxpy(float a, float const *x, float const *y, float *z, size_t n) {
    size_t i;
    for (i = 0; i + 3 < n; i += 4) {
        __m128 xi = _mm_loadu_ps(&x[i]);
        __m128 yi = _mm_loadu_ps(&y[i]);
        __m128 zi = _mm_add_ps(_mm_mul_ps(_mm_set1_ps(a), xi), yi);
        _mm_storeu_ps(&z[i], zi);
    }
    for (; i < n; i++) {
        z[i] = a * x[i] + y[i];
    }
}

// 1302 ns 0.64 cpi
[[gnu::optimize("O2")]] void saxpy(float a, float const *x, float const *y, float *z, size_t n) {
    size_t i;
    __m128 av = _mm_set1_ps(a);
    for (i = 0; i + 7 < n; i += 8) {
        __m128 xi = _mm_loadu_ps(&x[i]);
        __m128 yi = _mm_loadu_ps(&y[i]);
        __m128 xi2 = _mm_loadu_ps(&x[i + 4]);
        __m128 yi2 = _mm_loadu_ps(&y[i + 4]);
        __m128 zi = _mm_add_ps(_mm_mul_ps(av, xi), yi);
        __m128 zi2 = _mm_add_ps(_mm_mul_ps(av, xi2), yi2);
        _mm_storeu_ps(&z[i], zi);
        _mm_storeu_ps(&z[i + 4], zi2);
    }
    for (; i < n; i++) {
        z[i] = a * x[i] + y[i];
    }
}

// 1310 ns 0.64 cpi
[[gnu::optimize("O2")]] void saxpy(float a, float const *x, float const *y, float *z, size_t n) {
    size_t i;
    __m128 av = _mm_set1_ps(a);
    for (i = 0; i + 7 < n; i += 8) {
        __m128 xi = _mm_loadu_ps(&x[i]);
        __m128 xi2 = _mm_loadu_ps(&x[i + 4]);
        __m128 yi = _mm_loadu_ps(&y[i]);
        __m128 yi2 = _mm_loadu_ps(&y[i + 4]);
        __m128 zi = _mm_add_ps(_mm_mul_ps(av, xi), yi);
        __m128 zi2 = _mm_add_ps(_mm_mul_ps(av, xi2), yi2);
        _mm_storeu_ps(&z[i], zi);
        _mm_storeu_ps(&z[i + 4], zi2);
    }
    for (; i < n; i++) {
        z[i] = a * x[i] + y[i];
    }
}

#endif
