#include <cmath>
#include <immintrin.h>

#define WIDTH 4

// 以下是项目中的一段热点代码
// 会调用这个函数很多次，用来计算亚像素的像素值，方法是插值（具体算法可以不用管），里头的具体魔数我改了一下，因为是公司的代码，而且跟优化没有关系

void GetSubPixelValue(double* __restrict Rs, const double* __restrict preCaculatedParameter, int width, int height, double* __restrict Xs, double* __restrict Ys)
{
    auto c0_5 = _mm256_set1_pd(0.5);
    auto X = _mm256_loadu_pd(Xs);
    auto Y = _mm256_loadu_pd(Ys);
    auto xIndex2 = _mm256_cvttpd_epi32(_mm256_add_pd(X, c0_5));
    auto yIndex2 = _mm256_cvttpd_epi32(_mm256_add_pd(Y, c0_5));
    /* int xIndex[6], yIndex[6]; */
    /* xIndex[0] = int(X + 0.5) - 2; */
    /* yIndex[0] = int(Y + 0.5) - 2; */
    /* for (int i = 1; i < 5; i++) */
    /* { */
    /* 	xIndex[i] = xIndex[i - 1] + 1; */
    /* 	yIndex[i] = yIndex[i - 1] + 1; */
    /* } */

    auto c0_111 = _mm256_set1_pd(0.111);
    auto c0_222 = _mm256_set1_pd(-0.222);
    auto c0_233 = _mm256_set1_pd(0.233);
    auto c0_344 = _mm256_set1_pd(0.344);
    auto c0_15 = _mm256_set1_pd(0.15);
    auto c0_333 = _mm256_set1_pd(0.333);
    auto c0_444 = _mm256_set1_pd(0.444);
    auto c0_555 = _mm256_set1_pd(0.555);
    auto c0_0666 = _mm256_set1_pd(0.0666);
    auto c0_0777 = _mm256_set1_pd(0.0777);
    auto c0_345 = _mm256_set1_pd(0.345);
    auto c1o22 = _mm256_set1_pd(1.0 / 22.0);

    __m256d xWeight[5];
    __m256d yWeight[5];
    auto xw = _mm256_sub_pd(X, _mm256_cvtepi32_pd(xIndex2));
    auto xw2 = _mm256_mul_pd(xw, xw);
    auto xw3 = _mm256_mul_pd(xw, xw2);
    auto xw4 = _mm256_mul_pd(xw2, xw2);
    auto yw = _mm256_sub_pd(Y, _mm256_cvtepi32_pd(yIndex2));
    auto yw2 = _mm256_mul_pd(yw, yw);
    auto yw3 = _mm256_mul_pd(yw, yw2);
    auto yw4 = _mm256_mul_pd(yw2, yw2);
    xWeight[0] = _mm256_fmsub_pd(c0_111, xw4, _mm256_fmadd_pd(xw3, c1o22, _mm256_fmsub_pd(c0_233, xw2, _mm256_fmadd_pd(c0_344, xw, c0_345))));
    yWeight[0] = _mm256_fmsub_pd(c0_111, yw4, _mm256_fmadd_pd(yw3, c1o22, _mm256_fmsub_pd(c0_233, yw2, _mm256_fmadd_pd(c0_344, yw, c0_345))));
    xWeight[1] = _mm256_fmadd_pd(c0_333, xw3, _mm256_fmsub_pd(c0_444, xw2, _mm256_fmsub_pd(c0_555, xw, _mm256_fmadd_pd(c0_333, xw4, c0_0666))));
    yWeight[1] = _mm256_fmadd_pd(c0_333, yw3, _mm256_fmsub_pd(c0_444, yw2, _mm256_fmsub_pd(c0_555, yw, _mm256_fmadd_pd(c0_333, yw4, c0_0666))));
    xWeight[2] = _mm256_fmadd_pd(c0_15, xw4, _mm256_fmadd_pd(c0_222, xw2, c0_0777));
    yWeight[2] = _mm256_fmadd_pd(c0_15, yw4, _mm256_fmadd_pd(c0_222, yw2, c0_0777));
    xWeight[3] = _mm256_fmsub_pd(c0_444, xw2, _mm256_fmsub_pd(c0_333, xw4, _mm256_fmadd_pd(c0_333, xw3, _mm256_fmadd_pd(c0_555, xw, c0_0666))));
    yWeight[3] = _mm256_fmsub_pd(c0_444, yw2, _mm256_fmsub_pd(c0_333, yw4, _mm256_fmadd_pd(c0_333, yw3, _mm256_fmadd_pd(c0_555, yw, c0_0666))));
    xWeight[4] = _mm256_fmadd_pd(c0_111, xw4, _mm256_fmadd_pd(xw3, c1o22, _mm256_fmadd_pd(c0_233, xw2, _mm256_fmadd_pd(c0_344, xw, c0_345))));
    yWeight[4] = _mm256_fmadd_pd(c0_111, yw4, _mm256_fmadd_pd(yw3, c1o22, _mm256_fmadd_pd(c0_233, yw2, _mm256_fmadd_pd(c0_344, yw, c0_345))));
    
    int width2 = 2 * width - 2;
    int height2 = 2 * height - 2;
    auto v_width = _mm_set1_epi32(width);
    auto v_height = _mm_set1_epi32(height);
    auto v_width2 = _mm256_set1_pd((double)width2);
    auto v_height2 = _mm256_set1_pd((double)height2);
    auto v_width2i = _mm_set1_epi32(width2);
    auto v_height2i = _mm_set1_epi32(height2);
    __m128i xIndex[5];
    __m128i yIndex[5];
// 改了下面这两句会影响速度
    auto vi = _mm_set1_epi32(-2); 
    auto c1 = _mm_set1_epi32(1);

    for (int i = 0; i < 5; i++)
    {
        auto xIndexI = _mm_add_epi32(xIndex2, vi);
        xIndexI = _mm_abs_epi32(xIndexI);
        xIndexI = _mm_sub_epi32(xIndexI, _mm256_cvttpd_epi32(_mm256_mul_pd(v_width2, _mm256_floor_pd(_mm256_div_pd(_mm256_cvtepi32_pd(xIndexI), v_width2)))));
        xIndexI = _mm_blendv_epi8(xIndexI, _mm_sub_epi32(v_width2i, xIndexI), _mm_or_si128(_mm_cmpgt_epi32(xIndexI, v_width), _mm_cmpeq_epi32(xIndexI, v_width)));
        xIndex[i] = xIndexI;
        auto yIndexI = _mm_add_epi32(yIndex2, vi);
        yIndexI = _mm_abs_epi32(yIndexI);
        yIndexI = _mm_sub_epi32(yIndexI, _mm256_cvttpd_epi32(_mm256_mul_pd(v_height2, _mm256_floor_pd(_mm256_div_pd(_mm256_cvtepi32_pd(yIndexI), v_height2)))));
        yIndexI = _mm_blendv_epi8(yIndexI, _mm_sub_epi32(v_height2i, yIndexI), _mm_or_si128(_mm_cmpgt_epi32(yIndexI, v_height), _mm_cmpeq_epi32(yIndexI, v_height)));
        yIndexI = _mm_mullo_epi32(yIndexI, v_width);
        yIndex[i] = yIndexI;
        vi = _mm_add_epi32(vi, c1);
    }

    auto result = _mm256_setzero_pd();
    for (int i = 0; i < 5; i++)
    {
        auto rowResult = _mm256_setzero_pd();
        auto yIndexI = yIndex[i];
#pragma GCC unroll 5
        for (int j = 0; j < 5; j++)
        {
            auto factor = _mm256_i32gather_pd(preCaculatedParameter, _mm_add_epi32(yIndexI, xIndex[j]), 8); // 这个是8不是1
            rowResult = _mm256_fmadd_pd(factor, xWeight[j], rowResult);
        }
        result = _mm256_fmadd_pd(rowResult, yWeight[i], result);
    }
    _mm256_storeu_pd(Rs, result);
}

double GetSubPixelValueOrigin(const double* preCaculatedParameter, int width, int height, double X, double Y)
{
    int xIndex[6], yIndex[6];
    xIndex[0] = int(X + 0.5) - 2;
    yIndex[0] = int(Y + 0.5) - 2;
    for (int i = 1; i < 5; i++)
    {
        xIndex[i] = xIndex[i - 1] + 1;
        yIndex[i] = yIndex[i - 1] + 1;
    }

    double xWeight[5];
    {
        double w = X - (double)xIndex[2];
        double w2 = w * w;
        double w3 = w * w2;
        double w4 = w2 * w2;
        xWeight[0] = 0.111 * w4 - w3 / 22.0 + 0.233 * w2 - 0.344 * w + 0.345;
        xWeight[1] = -0.333 * w4 + 0.333 * w3 + 0.444 * w2 - 0.555 * w + 0.0666;
        xWeight[2] = 0.15 * w4 - 0.222 * w2 + 0.0777;
        xWeight[3] = -0.333 * w4 - 0.333 * w3 + 0.444 * w2 + 0.555 * w + 0.0666;
        xWeight[4] = 0.111 * w4 + w3 / 22.0 + 0.233 * w2 + 0.344 * w + 0.345;
    }

    // 下面这个计算方法和上面是一样的
    double yWeight[5];
    {
        double w = Y - (double)yIndex[2];
        double w2 = w * w;
        double w3 = w * w2;
        double w4 = w2 * w2;
        yWeight[0] = 0.111 * w4 - w3 / 22.0 + 0.233 * w2 - 0.344 * w + 0.345;
        yWeight[1] = -0.333 * w4 + 0.333 * w3 + 0.444 * w2 - 0.555 * w + 0.0666;
        yWeight[2] = 0.15 * w4 - 0.222 * w2 + 0.0777;
        yWeight[3] = -0.333 * w4 - 0.333 * w3 + 0.444 * w2 + 0.555 * w + 0.0666;
        yWeight[4] = 0.111 * w4 + w3 / 22.0 + 0.233 * w2 + 0.344 * w + 0.345;
    }

    int width2 = 2 * width - 2;
    int height2 = 2 * height - 2;
    for (int i = 0; i < 5; i++)
    {
        xIndex[i] = (xIndex[i] < 0) ? (-xIndex[i] - width2 * ((-xIndex[i]) / width2)) : (xIndex[i] - width2 * (xIndex[i] / width2));
        if (width <= xIndex[i])
            xIndex[i] = width2 - xIndex[i];

        yIndex[i] = (yIndex[i] < 0) ? (-yIndex[i] - height2 * ((-yIndex[i]) / height2)) : (yIndex[i] - height2 * (yIndex[i] / height2));
        if (height <= yIndex[i])
            yIndex[i] = height2 - yIndex[i];
    }

    double result = 0;
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            result += ((*(preCaculatedParameter + width * yIndex[i] + xIndex[j])) * xWeight[j] * yWeight[i]);
        }
    }
    return result;
}

#include <vector>
#include <chrono>
#include <iostream>

#define TICK(x) auto bench_##x = std::chrono::steady_clock::now();
#define TOCK(x) std::cerr<<#x ": "<<std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now()-bench_##x).count();std::cerr<<"秒\n";

double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

#define TEST_NUM 10000000
int main() {
    int width = 2048;
    int height = 2048;
    std::vector<double> preCaculatedParameter(width * height, 0.123); // 里面是预计算的数字
    for (size_t i = 0; i < preCaculatedParameter.size(); i++)
    {
        preCaculatedParameter[i] = fRand(0, 1);
    }

    std::vector<double> xs(TEST_NUM);
    std::vector<double> ys(TEST_NUM);
    for (size_t i = 0; i < xs.size(); i++)
    {
        xs[i] = rand() % 2048;
        ys[i] = rand() % 2048;
    }
    std::vector<double> result(TEST_NUM);

#if 0
    TICK(t);
    /* #pragma omp parallel for */
    for (size_t i = 0; i < TEST_NUM / WIDTH * WIDTH; i += WIDTH)
    {
        GetSubPixelValue(result.data() + i, preCaculatedParameter.data(), width, height, xs.data() + i, ys.data() + i);
    }
#ifdef __GNUC__
    asm volatile ("" ::: "cc", "memory");
#endif
    TOCK(t);
#else
   // 以下的测试是单独测试的，不会一起测试，也就是一次只测试一个程序
    std::vector<double> resultOrigin(TEST_NUM);
    TICK(t_origin);
    for (size_t i = 0; i < TEST_NUM; i++)
    {
        resultOrigin[i] = GetSubPixelValueOrigin(preCaculatedParameter.data(), width, height, xs[i], ys[i]);
    }
#ifdef __GNUC__
    asm volatile ("" ::: "cc", "memory");
#endif
    TOCK(t_origin);
#endif
    return 0;
}
