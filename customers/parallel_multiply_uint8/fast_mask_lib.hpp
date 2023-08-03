//g++ -mavx2 fast_simd.cpp 

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <immintrin.h>

static void multiply_cpp_avx2(char* img_HWN_ptr, bool* mask_HW_ptr, 
                  char* output_HWN_ptr, size_t size){
    __m256i one = _mm256_set1_epi8(1);

    const size_t width = 256 / 8; // =32 uint8 per batch
    size_t batch = size / width;
        
    uint8_t * vectorIMG = reinterpret_cast<uint8_t*>(img_HWN_ptr);
    uint8_t * vectorMASK = reinterpret_cast<uint8_t*>(mask_HW_ptr);
    uint8_t * vectorResult = reinterpret_cast<uint8_t*>(output_HWN_ptr);
    uint8_t * vectorResult_end = vectorResult + batch * width * 3;
    // 本可以直接上256b全部u8分量一起 shuffle 的，但是AVX的_mm256_shuffle很坑，他是高128b和低128b分别shuffle的
    //__m256i shuf1 = _mm256_setr_epi8(0 ,0 ,0 ,1 ,1 ,1 ,2 ,2 ,2 ,3 ,3 ,3 ,4 ,4 ,4 ,5 ,  // 0001112223334445
                                     //16,16,16,17,17,17,18,18,18,19,19,19,20,20,20,21); // GGGHHHIIIJJJKKKL
    //__m256i shuf2 = _mm256_setr_epi8(5 ,5 ,6 ,6 ,6 ,7 ,7 ,7 ,8 ,8 ,8 ,9 ,9 ,9 ,10,10,  // 55666777888999AA
                                     //26,27,27,27,28,28,28,29,29,29,30,30,30,31,31,31); // QRRRSSSTTTUUUVVV
    //__m256i shuf3 = _mm256_setr_epi8(10,11,11,11,12,12,12,13,13,13,14,14,14,15,15,15,  // ABBBCCCDDDEEEFFF
                                     //21,21,22,22,22,23,23,23,24,24,24,25,25,25,26,26); // LLMMMNNNOOOPPPQQ
    __m128i shuf1 = _mm_setr_epi8(0 ,0 ,0 ,1 ,1 ,1 ,2 ,2 ,2 ,3 ,3 ,3 ,4 ,4 ,4 ,5 );
    __m128i shuf2 = _mm_setr_epi8(5 ,5 ,6 ,6 ,6 ,7 ,7 ,7 ,8 ,8 ,8 ,9 ,9 ,9 ,10,10);
    __m128i shuf3 = _mm_setr_epi8(10,11,11,11,12,12,12,13,13,13,14,14,14,15,15,15);

    while (vectorResult != vectorResult_end) {
        __m256i vec_img_1 = _mm256_loadu_si256((__m256i*) vectorIMG);
        vectorIMG += width;
        __m256i vec_img_2 = _mm256_loadu_si256((__m256i*) vectorIMG);
        vectorIMG += width;
        __m256i vec_img_3  = _mm256_loadu_si256((__m256i*) vectorIMG);
        vectorIMG += width;
        __m256i vec_mask = _mm256_loadu_si256((__m256i*) vectorMASK);
        vectorMASK += width;
        // x86 没有提供 u8 的乘法指令，只能用 IMG & ~(MASK - 1) 来模拟 IMG * MASK
        __m256i maskorig = _mm256_sub_epi8(vec_mask, one);
        __m128i maskoriglo = _mm256_castsi256_si128(maskorig);      // 0123456789ABCDEF
        __m128i maskorighi = _mm256_extracti128_si256(maskorig, 1); // GHIJKLMNOPQRSTUV
        __m128i shufmask1lo = _mm_shuffle_epi8(maskoriglo, shuf1);
        __m128i shufmask1hi = _mm_shuffle_epi8(maskoriglo, shuf2);
        __m128i shufmask2lo = _mm_shuffle_epi8(maskoriglo, shuf3);
        __m128i shufmask2hi = _mm_shuffle_epi8(maskorighi, shuf1);
        __m128i shufmask3lo = _mm_shuffle_epi8(maskorighi, shuf2);
        __m128i shufmask3hi = _mm_shuffle_epi8(maskorighi, shuf3);
        __m256i shufmask1 = _mm256_setr_m128i(shufmask1lo, shufmask1hi);
        __m256i shufmask2 = _mm256_setr_m128i(shufmask2lo, shufmask2hi);
        __m256i shufmask3 = _mm256_setr_m128i(shufmask3lo, shufmask3hi);
        // REGISTER    01234567890123456789012345678901 01234567890123456789012345678901 01234567890123456789012345678901
        // img       = rgbrgbrgbrgbrgbrgbrgbrgbrgbrgbrg brgbrgbrgbrgbrgbrgbrgbrgbrgbrgbr gbrgbrgbrgbrgbrgbrgbrgbrgbrgbrgb
        // maskorig  = 0123456789ABCDEFGHIJKLMNOPQRSTUV
        // shufmask1 = 000111222333444555666777888999AA
        // shufmask2 = ABBBCCCDDDEEEFFFGGGHHHIIIJJJKKKL
        // shufmask3 = LLMMMNNNOOOPPPQQQRRRSSSTTTUUUVVV
        // 则 img_1 & shufmask1 就能起到和分别对 HWC 格式的前三分之一 rgb 进行 mask 一样的效果，依次类推
        __m256i result_1 = _mm256_andnot_si256(shufmask1, vec_img_1);
        __m256i result_2 = _mm256_andnot_si256(shufmask2, vec_img_2);
        __m256i result_3 = _mm256_andnot_si256(shufmask3, vec_img_3);
        _mm256_storeu_si256((__m256i*) vectorResult, result_1);
        vectorResult += width;
        _mm256_storeu_si256((__m256i*) vectorResult, result_2);
        vectorResult += width;
        _mm256_storeu_si256((__m256i*) vectorResult, result_3);
        vectorResult += width;
    }
    // 考虑边角料
    if (batch * width != size) [[unlikely]] {
        uint8_t * vectorResult_trueEnd = vectorResult + (size - batch * width) * 3;
        while (vectorResult != vectorResult_trueEnd) {
            uint8_t mask = *vectorMASK++;
            uint8_t r = *vectorIMG++;
            uint8_t g = *vectorIMG++;
            uint8_t b = *vectorIMG++;
            *vectorResult++ = r * mask;
            *vectorResult++ = g * mask;
            *vectorResult++ = b * mask;
        }
    }
}

static void multiply_cpp_avx2_full(char* img_HWN_ptr, bool* mask_HW_ptr, 
                  char* output_HWN_ptr, size_t size){
    __m256i one = _mm256_set1_epi8(1);

    const size_t width = 256 / 8; // =32 uint8 per batch
    size_t batch = size / width;
        
    uint8_t * vectorIMG = reinterpret_cast<uint8_t*>(img_HWN_ptr);
    uint8_t * vectorMASK = reinterpret_cast<uint8_t*>(mask_HW_ptr);
    uint8_t * vectorResult = reinterpret_cast<uint8_t*>(output_HWN_ptr);
    uint8_t * vectorResult_end = vectorResult + batch * width * 3;
    __m256i shuf12 = _mm256_setr_epi8(0 ,0 ,0 ,1 ,1 ,1 ,2 ,2 ,2 ,3 ,3 ,3 ,4 ,4 ,4 ,5 ,
                                      5 ,5 ,6 ,6 ,6 ,7 ,7 ,7 ,8 ,8 ,8 ,9 ,9 ,9 ,10,10);
    __m256i shuf23 = _mm256_setr_epi8(5 ,5 ,6 ,6 ,6 ,7 ,7 ,7 ,8 ,8 ,8 ,9 ,9 ,9 ,10,10,
                                      10,11,11,11,12,12,12,13,13,13,14,14,14,15,15,15);
    __m256i shuf31 = _mm256_setr_epi8(10,11,11,11,12,12,12,13,13,13,14,14,14,15,15,15,
                                      0 ,0 ,0 ,1 ,1 ,1 ,2 ,2 ,2 ,3 ,3 ,3 ,4 ,4 ,4 ,5 );
    // REGISTER    01234567890123456789012345678901 01234567890123456789012345678901 01234567890123456789012345678901
    // img       = rgbrgbrgbrgbrgbrgbrgbrgbrgbrgbrg brgbrgbrgbrgbrgbrgbrgbrgbrgbrgbr gbrgbrgbrgbrgbrgbrgbrgbrgbrgbrgb
    // maskorig  = 0123456789ABCDEFGHIJKLMNOPQRSTUV
    // shufmask1 = 000111222333444555666777888999AA
    // shufmask2 = ABBBCCCDDDEEEFFFGGGHHHIIIJJJKKKL
    // shufmask3 = LLMMMNNNOOOPPPQQQRRRSSSTTTUUUVVV
    // 则 img_1 & shufmask1 就能起到和分别对 HWC 格式的前三分之一 rgb 进行 mask 一样的效果，依次类推
    while (vectorResult != vectorResult_end) {
        __m256i vec_mask = _mm256_loadu_si256((__m256i*) vectorMASK);
        vectorMASK += width;
        __m256i maskorig = _mm256_sub_epi8(vec_mask, one);
        __m256i shufmask2 = _mm256_shuffle_epi8(maskorig, shuf31);

        __m256i vec_img_1 = _mm256_loadu_si256((__m256i*) vectorIMG);
        __m256i maskorig_ll = _mm256_broadcastsi128_si256(_mm256_castsi256_si128(maskorig));
        vectorIMG += width;
        __m256i shufmask1 = _mm256_shuffle_epi8(maskorig_ll, shuf12);
        __m256i result_1 = _mm256_andnot_si256(shufmask1, vec_img_1);
        _mm256_storeu_si256((__m256i*) vectorResult, result_1);
        vectorResult += width;

        __m256i maskorig_hh = _mm256_broadcastsi128_si256(_mm256_extracti128_si256(maskorig, 1));
        __m256i vec_img_2 = _mm256_loadu_si256((__m256i*) vectorIMG);
        __m256i result_2 = _mm256_andnot_si256(shufmask2, vec_img_2);
        vectorIMG += width;
        _mm256_storeu_si256((__m256i*) vectorResult, result_2);

        __m256i shufmask3 = _mm256_shuffle_epi8(maskorig_hh, shuf23);
        vectorResult += width;
        __m256i vec_img_3  = _mm256_loadu_si256((__m256i*) vectorIMG);
        vectorIMG += width;
        __m256i result_3 = _mm256_andnot_si256(shufmask3, vec_img_3);
        _mm256_storeu_si256((__m256i*) vectorResult, result_3);
        vectorResult += width;
    }
    // 考虑边角料
    if (batch * width != size) [[unlikely]] {
        uint8_t * vectorResult_trueEnd = vectorResult + (size - batch * width) * 3;
        while (vectorResult != vectorResult_trueEnd) {
            uint8_t mask = *vectorMASK++;
            uint8_t r = *vectorIMG++;
            uint8_t g = *vectorIMG++;
            uint8_t b = *vectorIMG++;
            *vectorResult++ = r * mask;
            *vectorResult++ = g * mask;
            *vectorResult++ = b * mask;
        }
    }
}

static void multiply_cpp_forloop(char* img_HWN_ptr, bool* mask_HW_ptr, 
                  char* output_HWN_ptr, size_t size){
    uint8_t * uptrIMG = reinterpret_cast<uint8_t*>(img_HWN_ptr);
    uint8_t * uptrMASK = reinterpret_cast<uint8_t*>(mask_HW_ptr);
    uint8_t * uptrResult = reinterpret_cast<uint8_t*>(output_HWN_ptr);
    for(size_t i=0; i<size; ++i){
        uptrResult[i*3] = uptrIMG[i*3] * uptrMASK[i];
        uptrResult[i*3+1] = uptrIMG[i*3+1] * uptrMASK[i];
        uptrResult[i*3+2] = uptrIMG[i*3+2] * uptrMASK[i];
    }
}

//multiply_cpp_forloop multiply_cpp_avx2 multiply_cpp_avx512
/* #define MULTIPLY_BY  multiply_cpp_avx2 */
static void multiply_cpp(char* img_HWN_ptr, bool* mask_HW_ptr, char* output_ptr,
                   int N, int H, int W, int mode, int parallel)
{
    if (N != 3) throw std::runtime_error("can only deal RGB24 for now");
    size_t size = H*W;
    auto multiply_by = multiply_cpp_forloop;
    if (mode == 1) {
        multiply_by = multiply_cpp_avx2;
    } else if (mode == 2) {
        multiply_by = multiply_cpp_avx2_full;
    }
    if (!parallel) {
        multiply_by(img_HWN_ptr, mask_HW_ptr, 
                    output_ptr, size);
    } else {
        const size_t chunk = 65536;
        #pragma omp parallel for
        for (size_t i = 0; i < size; i += chunk) {
            multiply_by(img_HWN_ptr + i*3, mask_HW_ptr + i,
                        output_ptr + i*3, std::min(chunk, size - i));
        }
    }
}
