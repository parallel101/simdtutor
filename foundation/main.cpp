#include <cstdint>
#include "print.h"
#include <cstddef>
#include <vector>
#include <random>
#include <vector>
#include <random>
#include <algorithm>
#include "dispatch_kernel.h"
#include "ScopeProfiler.h"
#include "simd_print.h"

#define _MM_SHUFFLER(w, x, y, z) _MM_SHUFFLE(z, y, x, w)

int main() {
    /* { */
    /*     __m128d m = _mm_setr_pd(1, 2); // double * 2 */
    /*     __m128d two = _mm_set1_pd(2); */
    /*     m = _mm_mul_pd(m, two); */
    /*     print(m); */
    /* } */
    /* { */
    /*     __m128 m = _mm_setr_ps(1.5f, 2.5f, 3.5f, 4.5f); // float * 4 */
    /*     print(m); */
    /*     __m128i mi = _mm_cvtps_epi32(m); */
    /*     print(mi); */
    /* } */
    /* { */
    /*     __m128i m = _mm_setr_epi16(1, 2, 3, 4, 5, 6, 7, 8); // char * 16, short * 8, int * 4, long long * 2 */
    /*     __m128i two = _mm_set1_epi16(2); */
    /*     m = _mm_mullo_epi16(m, two); */
    /*     print16(m); */
    /*  */
    /*     // a          = 100 100 100 100 */
    /*     // b          = 100 100 100 100 */
    /*     // mul(a,b)   = 10000   10000 */
    /*  */
    /*     // a          = 100 100 100 100 */
    /*     // b          = 100 100 100 100 */
    /*     // mullo(a,b) = 000 000 000 000 */
    /*  */
    /*     // a          = 100 100 100 100 */
    /*     // b          = 100 100 100 100 */
    /*     // mulhi(a,b) = 10  10  10  10 */
    /* } */
    /* { */
    /*     __m128i m = _mm_setr_epi32(1, 2, 3, 4); // int * 4 */
    /*     m = _mm_shuffle_epi32(m, 0b00011011); */
    /*     // new_m[0] = m[1] */
    /*     // new_m[1] = m[0] */
    /*     // new_m[2] = m[2] */
    /*     // new_m[3] = m[3] */
    /*     // m = new_m */
    /*     print(m); */
    /* } */
    /* { */
    /*     __m128 m = _mm_setr_ps(1, 2, 3, 4); */
    /*     __m128 n = _mm_shuffle_ps(m, m, _MM_SHUFFLER(1, 0, 3, 2)); */
    /*     m = _mm_add_ps(m, n); */
    /*     n = _mm_shuffle_ps(m, m, _MM_SHUFFLER(2, 3, 0, 1)); */
    /*     m = _mm_add_ss(m, n); */
    /*     print(_mm_cvtss_f32(m)); */
    /*     print(_mm_cvtss_f32(_mm_shuffle_ps(m, m, 1))); */
    /*     print(_mm_cvtss_f32(_mm_shuffle_ps(m, m, 2))); */
    /*     print(_mm_cvtss_f32(_mm_shuffle_ps(m, m, 3))); */
    /* } */
    {
        float a[4] = {1, 2, 3, 4};
        __m128 m = _mm_load_ps(a);
        print(m);
        float b[4] = {};
        _mm_storeu_ps(b, m);
        print(b);
    }
}
