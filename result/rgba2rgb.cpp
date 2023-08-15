#if _

// 584902 ns 1.13 cpi 23.16 GB/s
void rgba2rgb(uint8_t const *in_rgba, uint8_t *out_rgb, size_t n) {
    for (size_t i = 0; i < n; i++) {
        out_rgb[i * 3 + 0] = in_rgba[i * 4 + 0];
        out_rgb[i * 3 + 1] = in_rgba[i * 4 + 1];
        out_rgb[i * 3 + 2] = in_rgba[i * 4 + 2];
    }
}

// 533856 ns 1.03 cpi 25.37 GB/s
void rgba2rgb(uint8_t const *in_rgba, uint8_t *out_rgb, size_t n) {
    __m128i shuf = _mm_setr_epi8(0,1,2, 4,5,6, 8,9,10, 12,13,14, 0,0,0,0);
    auto in_rgba_end = in_rgba + ((n - 4) / 4 * 4) * 4;
    auto in_rgba_true_end = in_rgba + n * 4;
    while (in_rgba < in_rgba_end) {
        // rgbargbargbargba rgbargbargbargba
        // rgbrgbrgbrgbrgbr gbrgbrgb
        // ------------....
        __m128i v_rgba = _mm_loadu_si128((__m128i_u *)in_rgba);
        in_rgba += 16;

        __m128i v_rgb = _mm_shuffle_epi8(v_rgba, shuf);
        _mm_storeu_si128((__m128i_u *)out_rgb, v_rgb);
        out_rgb += 12;
    }

    while (in_rgba != in_rgba_true_end) {
        *out_rgb++ = *in_rgba++;
        *out_rgb++ = *in_rgba++;
        *out_rgb++ = *in_rgba++;
        in_rgba++;
    }
}

// 522225 ns 1.01 cpi 25.93 GB/s
void rgba2rgb(uint8_t const *in_rgba, uint8_t *out_rgb, size_t n) {
    __m128i shuf = _mm_setr_epi8(0,1,2, 4,5,6, 8,9,10, 12,13,14, 0,0,0,0);
    auto in_rgba_end = in_rgba + ((n - 4) / 4 * 4) * 4;
    auto in_rgba_true_end = in_rgba + n * 4;
    while (in_rgba < in_rgba_end) {
        // rgbargbargbargba rgbargbargbargba
        // rgbrgbrgbrgbrgbr gbrgbrgb
        // ------------....
        __m128i v_rgba = _mm_loadu_si128((__m128i_u *)in_rgba);
        in_rgba += 16;
        __m128i v2_rgba = _mm_loadu_si128((__m128i_u *)in_rgba);
        in_rgba += 16;

        __m128i v_rgb = _mm_shuffle_epi8(v_rgba, shuf);
        _mm_storeu_si128((__m128i_u *)out_rgb, v_rgb);
        out_rgb += 12;
        __m128i v2_rgb = _mm_shuffle_epi8(v2_rgba, shuf);
        _mm_storeu_si128((__m128i_u *)out_rgb, v2_rgb);
        out_rgb += 12;
    }

    while (in_rgba != in_rgba_true_end) {
        *out_rgb++ = *in_rgba++;
        *out_rgb++ = *in_rgba++;
        *out_rgb++ = *in_rgba++;
        in_rgba++;
    }
}

// 462828 ns 0.89 cpi 29.27 GB/s
void rgba2rgb(uint8_t const *in_rgba, uint8_t *out_rgb, size_t n) {
    __m128i shuf = _mm_setr_epi8(0,1,2, 4,5,6, 8,9,10, 12,13,14, 0,0,0,0);
    __m128i shuf2 = _mm_setr_epi8(5,6, 8,9,10, 12,13,14, 0,0,0,0, 0,1,2,4);
    auto in_rgba_end = in_rgba + ((n - 8) / 8 * 8) * 4;
    auto in_rgba_true_end = in_rgba + n * 4;
    while (in_rgba < in_rgba_end) {
        // rgbargbargbargba rgbargbargbargba
        // rgbrgbrgbrgbRGBR gbrgbrgb----RGBR
        // ------------....
        __m128i v_rgba = _mm_loadu_si128((__m128i_u *)in_rgba);
        in_rgba += 16;
        __m128i v2_rgba = _mm_loadu_si128((__m128i_u *)in_rgba);
        in_rgba += 16;
        __m128i v_rgb = _mm_shuffle_epi8(v_rgba, shuf);
        __m128i v2_rgb = _mm_shuffle_epi8(v2_rgba, shuf2);
        __m128i v1_rgb = _mm_blend_epi32(v_rgb, v2_rgb, 0b1000);
        _mm_storeu_si128((__m128i_u *)out_rgb, v1_rgb);
        out_rgb += 16;
        _mm_storeu_si64(out_rgb, v2_rgb);
        out_rgb += 8;
    }

    while (in_rgba != in_rgba_true_end) {
        *out_rgb++ = *in_rgba++;
        *out_rgb++ = *in_rgba++;
        *out_rgb++ = *in_rgba++;
        in_rgba++;
    }
}

// 418000 ns 0.8 cpi 32.43 GB/s
void rgba2rgb(uint8_t const *in_rgba, uint8_t *out_rgb, size_t n) {
    __m128i shuf1 = _mm_setr_epi8(0,1,2, 4,5,6, 8,9,10, 12,13,14, 0,0,0,0);
    __m128i shuf2 = _mm_setr_epi8(5,6, 8,9,10, 12,13,14, 0,0,0,0, 0,1,2,4);
    __m128i shuf3 = _mm_setr_epi8(10,12,13,14, 0,0,0,0, 0,1,2, 4,5,6, 8,9);
    __m128i shuf4 = _mm_setr_epi8(0,0,0,0, 0,1,2, 4,5,6, 8,9,10, 12,13,14);
    auto in_rgba_end = in_rgba + ((n - 8) / 8 * 8) * 4;
    auto in_rgba_true_end = in_rgba + n * 4;
    while (in_rgba < in_rgba_end) {
        // rgbargbargbargba RGBaRgbargbargba rgbargbargBaRGBa rgbargbargbargba
        // rgbrgbrgbrgb.... gbrgbrgb    RGBR BRGB    rgbrgbrg ....rgbrgbrgbrgb
        __m128i v1_rgba = _mm_loadu_si128((__m128i_u *)in_rgba);
        in_rgba += 16;
        __m128i v2_rgba = _mm_loadu_si128((__m128i_u *)in_rgba);
        in_rgba += 16;
        __m128i v3_rgba = _mm_loadu_si128((__m128i_u *)in_rgba);
        in_rgba += 16;
        __m128i v4_rgba = _mm_loadu_si128((__m128i_u *)in_rgba);
        in_rgba += 16;
        __m128i v1_rgb = _mm_shuffle_epi8(v1_rgba, shuf1);
        __m128i v2_rgb = _mm_shuffle_epi8(v2_rgba, shuf2);
        __m128i v3_rgb = _mm_shuffle_epi8(v3_rgba, shuf3);
        __m128i v4_rgb = _mm_shuffle_epi8(v4_rgba, shuf4);
        __m128i v1e_rgb = _mm_blend_epi32(v1_rgb, v2_rgb, 0b1000);
        __m128i v2e_rgb = _mm_blend_epi32(v2_rgb, v3_rgb, 0b1100);
        __m128i v3e_rgb = _mm_blend_epi32(v3_rgb, v4_rgb, 0b1110);
        _mm_storeu_si128((__m128i_u *)out_rgb, v1e_rgb);
        out_rgb += 16;
        _mm_storeu_si128((__m128i_u *)out_rgb, v2e_rgb);
        out_rgb += 16;
        _mm_storeu_si128((__m128i_u *)out_rgb, v3e_rgb);
        out_rgb += 16;
    }

    while (in_rgba != in_rgba_true_end) {
        *out_rgb++ = *in_rgba++;
        *out_rgb++ = *in_rgba++;
        *out_rgb++ = *in_rgba++;
        in_rgba++;
    }
}

// 441594 ns 0.85 cpi 30.69 GB/s
void rgba2rgb(uint8_t const *in_rgba, uint8_t *out_rgb, size_t n) {
    __m128i shuf1 = _mm_setr_epi8(0,1,2, 4,5,6, 8,9,10, 12,13,14, 0,0,0,0);
    __m128i shuf2 = _mm_setr_epi8(5,6, 8,9,10, 12,13,14, 0,0,0,0, 0,1,2,4);
    __m128i shuf3 = _mm_setr_epi8(10,12,13,14, 0,0,0,0, 0,1,2, 4,5,6, 8,9);
    __m128i shuf4 = _mm_setr_epi8(0,0,0,0, 0,1,2, 4,5,6, 8,9,10, 12,13,14);
    auto in_rgba_end = in_rgba + ((n - 16) / 16 * 16) * 4;
    auto in_rgba_true_end = in_rgba + n * 4;
    while (in_rgba < in_rgba_end) {
        // rgbargbargbargba RGBaRgbargbargba rgbargbargBaRGBa rgbargbargbargba
        // rgbrgbrgbrgb.... gbrgbrgb    RGBR BRGB    rgbrgbrg ....rgbrgbrgbrgb
        __m128i v1_rgba = _mm_loadu_si128((__m128i_u *)in_rgba);
        in_rgba += 16;
        __m128i v2_rgba = _mm_loadu_si128((__m128i_u *)in_rgba);
        in_rgba += 16;
        __m128i v3_rgba = _mm_loadu_si128((__m128i_u *)in_rgba);
        in_rgba += 16;
        __m128i v4_rgba = _mm_loadu_si128((__m128i_u *)in_rgba);
        in_rgba += 16;
        __m128i v1_rgb = _mm_shuffle_epi8(v1_rgba, shuf1);
        __m128i v2_rgb = _mm_shuffle_epi8(v2_rgba, shuf2);
        __m128i v3_rgb = _mm_shuffle_epi8(v3_rgba, shuf3);
        __m128i v4_rgb = _mm_shuffle_epi8(v4_rgba, shuf4);
        __m128i v1e_rgb = _mm_blend_epi32(v1_rgb, v2_rgb, 0b1000);
        __m128i v2e_rgb = _mm_castpd_si128(_mm_move_sd(_mm_castsi128_pd(v3_rgb), _mm_castsi128_pd(v2_rgb)));
        __m128i v3e_rgb = _mm_blend_epi32(v3_rgb, v4_rgb, 0b1110);
        _mm_storeu_si128((__m128i_u *)out_rgb, v1e_rgb);
        out_rgb += 16;
        _mm_storeu_si128((__m128i_u *)out_rgb, v2e_rgb);
        out_rgb += 16;
        _mm_storeu_si128((__m128i_u *)out_rgb, v3e_rgb);
        out_rgb += 16;
    }

    while (in_rgba != in_rgba_true_end) {
        *out_rgb++ = *in_rgba++;
        *out_rgb++ = *in_rgba++;
        *out_rgb++ = *in_rgba++;
        in_rgba++;
    }
}

// 423331 ns 0.81 cpi 32.0 GB/s
void rgba2rgb(uint8_t const *in_rgba, uint8_t *out_rgb, size_t n) {
    __m128i shuf1 = _mm_setr_epi8(0,1,2, 4,5,6, 8,9,10, 12,13,14, 0,0,0,0);
    __m128i shuf2 = _mm_setr_epi8(5,6, 8,9,10, 12,13,14, 0,0,0,0, 0,1,2,4);
    __m128i shuf3 = _mm_setr_epi8(10,12,13,14, 0,0,0,0, 0,1,2, 4,5,6, 8,9);
    __m128i shuf4 = _mm_setr_epi8(0,0,0,0, 0,1,2, 4,5,6, 8,9,10, 12,13,14);
    auto in_rgba_end = in_rgba + ((n - 16) / 16 * 16) * 4;
    auto in_rgba_true_end = in_rgba + n * 4;
    while (in_rgba < in_rgba_end) {
        // rgbargbargbargba RGBaRgbargbargba rgbargbargBaRGBa rgbargbargbargba
        // rgbrgbrgbrgb.... gbrgbrgb    RGBR BRGB    rgbrgbrg ....rgbrgbrgbrgb
        __m128i v1_rgba = _mm_loadu_si128((__m128i_u *)in_rgba);
        in_rgba += 16;
        __m128i v1_rgb = _mm_shuffle_epi8(v1_rgba, shuf1);
        __m128i v2_rgba = _mm_loadu_si128((__m128i_u *)in_rgba);
        in_rgba += 16;
        __m128i v2_rgb = _mm_shuffle_epi8(v2_rgba, shuf2);
        __m128i v1e_rgb = _mm_blend_epi32(v1_rgb, v2_rgb, 0b1000);
        __m128i v3_rgba = _mm_loadu_si128((__m128i_u *)in_rgba);
        in_rgba += 16;
        __m128i v3_rgb = _mm_shuffle_epi8(v3_rgba, shuf3);
        _mm_storeu_si128((__m128i_u *)out_rgb, v1e_rgb);
        out_rgb += 16;
        __m128i v2e_rgb = _mm_castpd_si128(_mm_move_sd(_mm_castsi128_pd(v3_rgb), _mm_castsi128_pd(v2_rgb)));
        __m128i v4_rgba = _mm_loadu_si128((__m128i_u *)in_rgba);
        in_rgba += 16;
        __m128i v4_rgb = _mm_shuffle_epi8(v4_rgba, shuf4);
        _mm_storeu_si128((__m128i_u *)out_rgb, v2e_rgb);
        out_rgb += 16;
        __m128i v3e_rgb = _mm_blend_epi32(v3_rgb, v4_rgb, 0b1110);
        _mm_storeu_si128((__m128i_u *)out_rgb, v3e_rgb);
        out_rgb += 16;
    }

    while (in_rgba != in_rgba_true_end) {
        *out_rgb++ = *in_rgba++;
        *out_rgb++ = *in_rgba++;
        *out_rgb++ = *in_rgba++;
        in_rgba++;
    }
}

// 408279 ns 0.79 cpi 33.18 GB/s
void rgba2rgb(uint8_t const *in_rgba, uint8_t *out_rgb, size_t n) {
    __m128i shuf1 = _mm_setr_epi8(0,1,2, 4,5,6, 8,9,10, 12,13,14, 0,0,0,0);
    __m128i shuf2 = _mm_setr_epi8(5,6, 8,9,10, 12,13,14, 0,0,0,0, 0,1,2,4);
    __m128i shuf3 = _mm_setr_epi8(10,12,13,14, 0,0,0,0, 0,1,2, 4,5,6, 8,9);
    __m128i shuf4 = _mm_setr_epi8(0,0,0,0, 0,1,2, 4,5,6, 8,9,10, 12,13,14);
    auto in_rgba_end = in_rgba + ((n - 16) / 16 * 16) * 4;
    auto in_rgba_true_end = in_rgba + n * 4;
    while (in_rgba < in_rgba_end) {
        // rgbargbargbargba RGBaRgbargbargba rgbargbargBaRGBa rgbargbargbargba
        // rgbrgbrgbrgb.... gbrgbrgb    RGBR BRGB    rgbrgbrg ....rgbrgbrgbrgb
        __m128i v1_rgba = _mm_loadu_si128((__m128i_u *)in_rgba);
        in_rgba += 16;
        __m128i v2_rgba = _mm_loadu_si128((__m128i_u *)in_rgba);
        in_rgba += 16;
        __m128i v3_rgba = _mm_loadu_si128((__m128i_u *)in_rgba);
        in_rgba += 16;
        __m128i v4_rgba = _mm_loadu_si128((__m128i_u *)in_rgba);
        in_rgba += 16;
        __m128i v1_rgb = _mm_shuffle_epi8(v1_rgba, shuf1);
        __m128i v2_rgb = _mm_shuffle_epi8(v2_rgba, shuf2);
        __m128i v3_rgb = _mm_shuffle_epi8(v3_rgba, shuf3);
        __m128i v4_rgb = _mm_shuffle_epi8(v4_rgba, shuf4);
        __m128i v1e_rgb = _mm_blend_epi32(v1_rgb, v2_rgb, 0b1000);
        __m128i v2e_rgb = _mm_blend_epi32(v2_rgb, v3_rgb, 0b1100);
        __m128i v3e_rgb = _mm_blend_epi32(v3_rgb, v4_rgb, 0b1110);
        _mm_storeu_si128((__m128i_u *)out_rgb, v1e_rgb);
        out_rgb += 16;
        _mm_storeu_si128((__m128i_u *)out_rgb, v2e_rgb);
        out_rgb += 16;
        _mm_storeu_si128((__m128i_u *)out_rgb, v3e_rgb);
        out_rgb += 16;
    }

    while (in_rgba != in_rgba_true_end) {
        *out_rgb++ = *in_rgba++;
        *out_rgb++ = *in_rgba++;
        *out_rgb++ = *in_rgba++;
        in_rgba++;
    }
}

// 425169 ns 0.82 cpi 31.87 GB/s
void rgba2rgb(uint8_t const *in_rgba, uint8_t *out_rgb, size_t n) {
    __m128i shuf1 = _mm_setr_epi8(0,1,2, 4,5,6, 8,9,10, 12,13,14, 0,0,0,0);
    __m128i shuf2 = _mm_setr_epi8(5,6, 8,9,10, 12,13,14, 0,0,0,0, 0,1,2,4);
    __m128i shuf3 = _mm_setr_epi8(10,12,13,14, 0,0,0,0, 0,1,2, 4,5,6, 8,9);
    __m128i shuf4 = _mm_setr_epi8(0,0,0,0, 0,1,2, 4,5,6, 8,9,10, 12,13,14);
    auto in_rgba_end = in_rgba + ((n - 16) / 16 * 16) * 4;
    auto in_rgba_true_end = in_rgba + n * 4;
    while (in_rgba < in_rgba_end) {
        // rgbargbargbargba RGBaRgbargbargba rgbargbargBaRGBa rgbargbargbargba
        // rgbrgbrgbrgb.... gbrgbrgb    RGBR BRGB    rgbrgbrg ....rgbrgbrgbrgb
        __m128i v1_rgba = _mm_loadu_si128((__m128i_u *)in_rgba);
        in_rgba += 16;
        __m128i v1_rgb = _mm_shuffle_epi8(v1_rgba, shuf1);
        __m128i v2_rgba = _mm_loadu_si128((__m128i_u *)in_rgba);
        in_rgba += 16;
        __m128i v2_rgb = _mm_shuffle_epi8(v2_rgba, shuf2);
        __m128i v3_rgba = _mm_loadu_si128((__m128i_u *)in_rgba);
        in_rgba += 16;
        __m128i v1e_rgb = _mm_blend_epi32(v1_rgb, v2_rgb, 0b1000);
        _mm_storeu_si128((__m128i_u *)out_rgb, v1e_rgb);
        out_rgb += 16;
        __m128i v4_rgba = _mm_loadu_si128((__m128i_u *)in_rgba);
        in_rgba += 16;
        __m128i v3_rgb = _mm_shuffle_epi8(v3_rgba, shuf3);
        __m128i v2e_rgb = _mm_blend_epi32(v2_rgb, v3_rgb, 0b1100);
        __m128i v4_rgb = _mm_shuffle_epi8(v4_rgba, shuf4);
        _mm_storeu_si128((__m128i_u *)out_rgb, v2e_rgb);
        out_rgb += 16;
        __m128i v3e_rgb = _mm_blend_epi32(v3_rgb, v4_rgb, 0b1110);
        _mm_storeu_si128((__m128i_u *)out_rgb, v3e_rgb);
        out_rgb += 16;
    }

    while (in_rgba != in_rgba_true_end) {
        *out_rgb++ = *in_rgba++;
        *out_rgb++ = *in_rgba++;
        *out_rgb++ = *in_rgba++;
        in_rgba++;
    }
}

#endif
