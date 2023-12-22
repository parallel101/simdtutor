#include <cstring>
#include <immintrin.h>
#include <chrono>
#include <iostream>
#define TICK(x) auto bench_##x = std::chrono::steady_clock::now();
#define TOCK(x) std::cerr<<#x ": "<<std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now()-bench_##x).count();std::cerr<<"秒\n";

typedef signed short s16;
typedef signed char  s8;
typedef s16     pel;
typedef  const  s8 tab_s8;

tab_s8 filter_offset_list[4] = { 32, 32 ,64, 32 };
tab_s8 filter_bits_list[4] = { 6, 6, 7, 6 };
s16 tbl_filt_list[3][4][4] =
{
        {
            { 10, 100, 11, -3 },
            { 1, 65, 15, -4 },
            { -4, 21, 44, -5 },
            { -9, 65, 24, -3 }
        },
        {
            { -5, 21, 55, -6 },
            { -2, 11, 23, -3 },
            { -4,  9, 64, -6 },
            { 2,  3, 42, -6 }
        },
        {
            { -7, 5, 55, -6 },
            { -3, 11, 3, -3 },
            { -7,  5, 64, -6 },
            { 2,  3, 73, -6 }
        }
};

/* #ifdef __GNUC__ */
/* __attribute__((__always_inline__)) */
/* #endif */
static void convolve(int16_t const *__restrict in, int16_t *__restrict out, int16_t const *__restrict filter, size_t n, uint8_t shift, uint16_t offset) {
    for (size_t i = 0; i < n; i++) {
        uint16_t tmp = 0;
        for (size_t j = 0; j < 4; j++) {
            tmp += in[i + j] * filter[j];
        }
        out[i] = tmp >> shift;
    }
}

/* #ifdef __GNUC__ */
/* __attribute__((__always_inline__)) */
/* #endif */
void s16copy(int16_t *__restrict out, int16_t const *__restrict in, size_t n) {
    for (size_t i = 0; i < n; i++) {
        out[i] = in[i];
    }
}

static void i22(pel*__restrict src, s16*__restrict dst, int i_dst, int width, int height, const int td)
{
    const int is_small = width * height <= ((td - 1) ? 64 : 32);
    s16 *__restrict filter;
    s8 offset, shift_r;

    // i < td
    offset = filter_offset_list[is_small + 1];
    shift_r = filter_bits_list[is_small + 1];
    s16 col_0[64], col_1_td2[64];

    filter = tbl_filt_list[is_small + 1][1];
    convolve(src - height - 1, col_0, filter, height, shift_r, offset);
    /* for (int j = 0; j < height; j++) { */
    /*     col_0[j] = (s16)(( */
    /*         src[j - height - 1] * filter[0] + */
    /*         src[j - height - 1 + 1] * filter[1] + */
    /*         src[j - height - 1 + 2] * filter[2] + */
    /*         src[j - height - 1 + 3] * filter[3] + */
    /*         offset) >> shift_r); */
    /* } */
    if (2 == td) {
        filter = tbl_filt_list[is_small + 1][2];
        convolve(src - height - 1, col_1_td2, filter, height, shift_r, offset);
        /* for (int j = 0; j < height; j++) { */
        /*     col_1_td2[j] = (s16)(( */
        /*         src[j - height - 1] * filter[0] + */
        /*         src[j - height - 1 + 1] * filter[1] + */
        /*         src[j - height - 1 + 2] * filter[2] + */
        /*         src[j - height - 1 + 3] * filter[3] + */
        /*         offset) >> shift_r); */
        /* } */
    }

    // i >= td
    offset = filter_offset_list[is_small];
    shift_r = filter_bits_list[is_small];

    // i >= td, else(td=1)
    int rem_rl = 3 - td; // remainder of ref_left
    s16 ref_left[254];
    for (int k = 0; k < rem_rl; k++) {
        filter = tbl_filt_list[is_small][(k + 1 + td)];
        ref_left[k] = (s16)((
            src[-height - 1] * filter[0] +
            src[-height - 1 + 1] * filter[1] +
            src[-height - 1 + 2] * filter[2] +
            src[-height - 1 + 3] * filter[3] +
            offset) >> shift_r);
    }

    if (width > 4) {
        for (int j = 0; j < (height - 1); j++) {
            filter = tbl_filt_list[is_small][((3 + 1) % 4)];
            ref_left[rem_rl + 4 * j] = (s16)((
                src[j - height] * filter[0] +
                src[j - height + 1] * filter[1] +
                src[j - height + 2] * filter[2] +
                src[j - height + 3] * filter[3] +
                offset) >> shift_r);


            filter = tbl_filt_list[is_small][((4 + 1) % 4)];
            ref_left[rem_rl + 4 * j + 1] = (s16)((
                src[j - height] * filter[0] +
                src[j - height + 1] * filter[1] +
                src[j - height + 2] * filter[2] +
                src[j - height + 3] * filter[3] +
                offset) >> shift_r);

            filter = tbl_filt_list[is_small][((5 + 1) % 4)];
            ref_left[rem_rl + 4 * j + 2] = (s16)((
                src[j - height] * filter[0] +
                src[j - height + 1] * filter[1] +
                src[j - height + 2] * filter[2] +
                src[j - height + 3] * filter[3] +
                offset) >> shift_r);

            filter = tbl_filt_list[is_small][((6 + 1) % 4)];
            ref_left[rem_rl + 4 * j + 3] = (s16)((
                src[j - height] * filter[0] +
                src[j - height + 1] * filter[1] +
                src[j - height + 2] * filter[2] +
                src[j - height + 3] * filter[3] +
                offset) >> shift_r);
        }
    }
    else {
        if (1 == td) {
            for (int j = 0; j < (height - 1); j++) {
                filter = tbl_filt_list[is_small][((3 + 1) % 4)];
                ref_left[rem_rl + 3 * j] = (s16)((
                    src[j - height] * filter[0] +
                    src[j - height + 1] * filter[1] +
                    src[j - height + 2] * filter[2] +
                    src[j - height + 3] * filter[3] +
                    offset) >> shift_r);


                filter = tbl_filt_list[is_small][((1 + 1) % 4)];
                ref_left[rem_rl + 3 * j + 1] = (s16)((
                    src[j - height] * filter[0] +
                    src[j - height + 1] * filter[1] +
                    src[j - height + 2] * filter[2] +
                    src[j - height + 3] * filter[3] +
                    offset) >> shift_r);

                filter = tbl_filt_list[is_small][((2 + 1) % 4)];
                ref_left[rem_rl + 3 * j + 2] = (s16)((
                    src[j - height] * filter[0] +
                    src[j - height + 1] * filter[1] +
                    src[j - height + 2] * filter[2] +
                    src[j - height + 3] * filter[3] +
                    offset) >> shift_r);
            }
        }
        else {
            for (int j = 0; j < (height - 1); j++) {
                filter = tbl_filt_list[is_small][((3 + 1) % 4)];
                ref_left[rem_rl + 2 * j] = (s16)((
                    src[j - height] * filter[0] +
                    src[j - height + 1] * filter[1] +
                    src[j - height + 2] * filter[2] +
                    src[j - height + 3] * filter[3] +
                    offset) >> shift_r);


                filter = tbl_filt_list[is_small][((2 + 1) % 4)];
                ref_left[rem_rl + 2 * j + 1] = (s16)((
                    src[j - height] * filter[0] +
                    src[j - height + 1] * filter[1] +
                    src[j - height + 2] * filter[2] +
                    src[j - height + 3] * filter[3] +
                    offset) >> shift_r);
            }
        }
    }

    // i >= td, if
    filter = tbl_filt_list[is_small][0];
    s16 ref_above[61];
    for (int i = 0; i < (width - 3); i++) {
        ref_above[i] = (s16)((
            src[i + 1] * filter[0] +
            src[i] * filter[1] +
            src[i - 1] * filter[2] +
            src[i - 2] * filter[3] +
            offset) >> shift_r);
    }

    // store
    if (width > 4) {
        for (int j = 0; j < height; j++) {
            dst[0] = col_0[height - 1 - j];
        }
        if (2 == td) {
            for (int j = 0; j < height; j++) {
                dst[1] = col_1_td2[height - 1 - j];
            }
        }

        auto mid = std::min((width - 3) / 4, height);
        auto rlp = ref_left + (4 * (height - 1) + rem_rl - 1) - (rem_rl - 1);
        s16copy(dst + td, rlp - 4 * mid, rem_rl + 4 * mid);
        for (int j = 0; j < mid; j++) {
            s16copy(dst + 3 + 4 * j, ref_above, width - 3 - 4 * j);
        }
        for (int j = mid; j < height; j++) {
            s16copy(dst + td, rlp - 4 * j, width - td);
        }

        dst += i_dst;
    }
    else {
        for (int j = 0; j < height; j++) {
            dst[0] = col_0[height - 1 - j];
            if (2 == td) {
                dst[1] = col_1_td2[height - 1 - j];
            }
            if (0 == j) {
                memcpy(dst + td, ref_left + (rem_rl + 1) * (height - 1) + rem_rl - 1 - (rem_rl - 1), rem_rl * sizeof(s16));
                dst[3] = ref_above[0];
            }
            else {
                memcpy(dst + td, ref_left + (rem_rl + 1) * (height - 1) + rem_rl - 1 - (j * (rem_rl + 1) + rem_rl - 1), (rem_rl + 1) * sizeof(s16));
            }

            dst += i_dst;
        }
    }

}

int main(void) {
    pel src[3][64 * 64];
    s16 d[64 * 64];

    int w, h, l = 64, td = 2;
    int whl_size[7] = { 4, 8, 12, 16, 24, 32, 64 };

    for (int k = 0; k < 64 * 64; k++)
    {
        int i_pixel = k % 1024;
        src[0][k] = i_pixel;
        src[1][k] = i_pixel;
        src[2][k] = i_pixel;
    }

    TICK(i22);
    for (int times = 0; times < 100000; times++) {
        for (int j = 0; j < 7; j++) {
            for (int f = 0; f < 7; f++) {
                w = whl_size[j], h = whl_size[f];
                i22(src[1], d, l, w, h, td);
            }
        }
#ifdef __GNUC__
        asm volatile ("" ::: "cc", "memory");
#endif
    }
    TOCK(i22);

    return 0;
}
