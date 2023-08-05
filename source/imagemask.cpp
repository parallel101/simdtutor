#include <benchmark/benchmark.h>
#include <gtest/gtest.h>
#include <random>
#include <memory>
#include <cstddef>
#include <immintrin.h>

// BEGIN CODE
void imagemask(uint8_t const *img, uint8_t const *mask, size_t n, uint8_t *out) {
    __m256i one = _mm256_set1_epi8(1);

    const size_t width = 256 / 8; // =32 uint8 per batch
    size_t batch = n / width;
        
    uint8_t const *vectorIMG = reinterpret_cast<uint8_t const *>(img);
    uint8_t const *vectorMASK = reinterpret_cast<uint8_t const *>(mask);
    uint8_t * vectorResult = reinterpret_cast<uint8_t *>(out);
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
    if (batch * width != n) [[unlikely]] {
        uint8_t * vectorResult_trueEnd = vectorResult + (n - batch * width) * 3;
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
// END CODE

static void bench(benchmark::State &s) {
    const auto n = size_t(8192);
    auto img = std::vector<uint8_t>(n * 3);
    auto mask = std::vector<uint8_t>(n);
    auto out = std::vector<uint8_t>(n * 3);
    auto out2 = std::vector<uint8_t>(n * 3);
    std::generate(img.begin(), img.end(), [uni = std::uniform_int_distribution<uint8_t>(), rng = std::mt19937()] () mutable { return uni(rng); });
    std::generate(mask.begin(), mask.end(), [uni = std::uniform_int_distribution<uint8_t>(), rng = std::mt19937()] () mutable { return uni(rng); });
    for (auto _: s) {
        imagemask(img.data(), mask.data(), n, out.data());
        benchmark::DoNotOptimize(img);
        benchmark::DoNotOptimize(mask);
        benchmark::DoNotOptimize(out);
    }
    s.SetItemsProcessed(n * s.iterations());
    s.SetBytesProcessed(n * 7 * s.iterations());
}
BENCHMARK(bench);

void scalar_imagemask(uint8_t const *img, uint8_t const *mask, size_t n, uint8_t *out) {
    for (size_t i = 0; i < n; i++) {
        out[i * 3 + 0] = img[i * 3 + 0] & mask[i];
        out[i * 3 + 1] = img[i * 3 + 1] & mask[i];
        out[i * 3 + 2] = img[i * 3 + 2] & mask[i];
    }
}

TEST(MySuite, MyTest) {
    const auto n = size_t(16384);
    auto img = std::vector<uint8_t>(n * 3);
    auto mask = std::vector<uint8_t>(n);
    auto out = std::vector<uint8_t>(n * 3);
    auto out2 = std::vector<uint8_t>(n * 3);
    std::generate(img.begin(), img.end(), [uni = std::uniform_real_distribution<float>(), rng = std::mt19937()] () mutable { return uni(rng); });
    std::generate(mask.begin(), mask.end(), [uni = std::uniform_real_distribution<float>(), rng = std::mt19937()] () mutable { return uni(rng); });
    imagemask(img.data(), mask.data(), n, out.data());
    scalar_imagemask(img.data(), mask.data(), n, out2.data());
    for (size_t i = 0; i < n * 3; i++) {
        EXPECT_EQ(out[i], out2[i]);
        if (out[i] != out2[i]) break;
    }
}
