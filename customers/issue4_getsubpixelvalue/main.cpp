#include <cmath>
#include <immintrin.h>

#define WIDTH 4

// 以下是项目中的一段热点代码
// 会调用这个函数很多次，用来计算亚像素的像素值，方法是插值（具体算法可以不用管），里头的具体魔数我改了一下，因为是公司的代码，而且跟优化没有关系

void GetSubPixelValue(double *__restrict Rs, const double*__restrict preCaculatedParameter, int width, int height, double *__restrict Xs, double *__restrict Ys)
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
    auto c0_222 = _mm256_set1_pd(0.222);
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
    auto yw = _mm256_sub_pd(X, _mm256_cvtepi32_pd(yIndex2));
    auto yw2 = _mm256_mul_pd(yw, yw);
    auto yw3 = _mm256_mul_pd(yw, yw2);
    auto yw4 = _mm256_mul_pd(yw2, yw2);
    xWeight[0] = _mm256_fmsub_pd(c0_111, xw4, _mm256_fmadd_pd(xw3, c1o22, _mm256_fmsub_pd(c0_233, xw2, _mm256_fmadd_pd(c0_344, xw, c0_345))));
    yWeight[0] = _mm256_fmsub_pd(c0_111, yw4, _mm256_fmadd_pd(yw3, c1o22, _mm256_fmsub_pd(c0_233, yw2, _mm256_fmadd_pd(c0_344, yw, c0_345))));
    xWeight[1] = _mm256_fmadd_pd(c0_333, xw3, _mm256_fmsub_pd(c0_444, xw2, _mm256_fmsub_pd(c0_555, xw, _mm256_fmadd_pd(c0_333, xw4, c0_0666))));
    yWeight[1] = _mm256_fmadd_pd(c0_333, yw3, _mm256_fmsub_pd(c0_444, yw2, _mm256_fmsub_pd(c0_555, yw, _mm256_fmadd_pd(c0_333, yw4, c0_0666))));
    xWeight[2] = _mm256_fmsub_pd(c0_15, xw4, _mm256_fmadd_pd(c0_222, xw2, c0_0777));
    yWeight[2] = _mm256_fmsub_pd(c0_15, yw4, _mm256_fmadd_pd(c0_222, yw2, c0_0777));
    xWeight[3] = _mm256_fmsub_pd(c0_444, xw2, _mm256_fmsub_pd(c0_333, xw4, _mm256_fmadd_pd(c0_333, xw3, _mm256_fmadd_pd(c0_555, xw, c0_0666))));
    yWeight[3] = _mm256_fmsub_pd(c0_444, yw2, _mm256_fmsub_pd(c0_333, yw4, _mm256_fmadd_pd(c0_333, yw3, _mm256_fmadd_pd(c0_555, yw, c0_0666))));
    xWeight[4] = _mm256_fmadd_pd(c0_111, xw4, _mm256_fmadd_pd(xw3, c1o22, _mm256_fmadd_pd(c0_233, xw2, _mm256_fmadd_pd(c0_344, xw, c0_345))));
    yWeight[4] = _mm256_fmadd_pd(c0_111, yw4, _mm256_fmadd_pd(yw3, c1o22, _mm256_fmadd_pd(c0_233, yw2, _mm256_fmadd_pd(c0_344, yw, c0_345))));

	/* int width2 = 2 * width - 2; */
	/* int height2 = 2 * height - 2; */
	/*     auto v_width = _mm_set1_epi32(width); */
	/*     auto v_height = _mm_set1_epi32(height); */
	/*     auto v_width2 = _mm256_set1_pd((double)width2); */
	/*     auto v_height2 = _mm256_set1_pd((double)height2); */
	/*     auto v_width2i = _mm_set1_epi32(width2); */
	/*     auto v_height2i = _mm_set1_epi32(height2); */
	__m128i xIndex[5];
	__m128i yIndex[5];
    auto vi = _mm_setzero_si128();
    auto c1 = _mm_set1_epi64x(1);
    auto wmask = _mm_set1_epi32(2047);
    auto hmask = _mm_set1_epi32(2047);
      #pragma GCC unroll 5
	for (int i = 0; i < 5; i++)
	{
        auto xIndexI = _mm_sub_epi32(xIndex2, vi);
        vi = _mm_add_epi32(vi, c1);
        xIndexI = _mm_and_si128(xIndexI, wmask);
		/* xIndexI = _mm_abs_epi32(xIndexI); */
		/* xIndexI = _mm_sub_epi32(xIndexI, _mm256_cvttpd_epi32(_mm256_mul_pd(v_width2, _mm256_floor_pd(_mm256_div_pd(_mm256_cvtepi32_pd(xIndexI), v_width2))))); */
		/*         xIndexI = _mm_blendv_epi8(xIndexI, _mm_sub_epi32(v_width2i, xIndexI), _mm_or_si128(_mm_cmpgt_epi32(xIndexI, v_width), _mm_cmpeq_epi32(xIndexI, v_width))); */
        xIndex[i] = xIndexI;
        auto yIndexI = yIndex2;
        yIndexI = _mm_and_si128(xIndexI, hmask);
		/* yIndexI = _mm_abs_epi32(yIndexI); */
		/* yIndexI = _mm_sub_epi32(yIndexI, _mm256_cvttpd_epi32(_mm256_mul_pd(v_height2, _mm256_floor_pd(_mm256_div_pd(_mm256_cvtepi32_pd(yIndexI), v_height2))))); */
		/*         yIndexI = _mm_blendv_epi8(yIndexI, _mm_sub_epi32(v_height2i, yIndexI), _mm_or_si128(_mm_cmpgt_epi32(yIndexI, v_height), _mm_cmpeq_epi32(yIndexI, v_height))); */
        yIndexI = _mm_slli_epi32(yIndexI, 11); // *2048
        yIndex[i] = yIndexI;
	}

    auto result = _mm256_setzero_pd();
	for (int i = 0; i < 5; i++)
	{
        auto rowResult = _mm256_setzero_pd();
        auto yIndexI = yIndex[i];
      #pragma GCC unroll 5
		for (int j = 0; j < 5; j++)
		{
            auto factor = _mm256_i32gather_pd(preCaculatedParameter, _mm_add_epi32(yIndexI, xIndex[j]), 1);
            rowResult = _mm256_fmadd_pd(rowResult, factor, xWeight[j]);
		}
        result = _mm256_fmadd_pd(result, rowResult, yWeight[i]);
	}
    _mm256_storeu_pd(Rs, result);
}

#include <vector>
#include <chrono>
#include <iostream>

#define TICK(x) auto bench_##x = std::chrono::steady_clock::now();
#define TOCK(x) std::cerr<<#x ": "<<std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now()-bench_##x).count();std::cerr<<"秒\n";


#define TEST_NUM 10000000
int main() {
	int width = 2048;
	int height = 2048;
	std::vector<double> preCaculatedParameter(width * height, 0.123); // 里面是预计算的数字
	std::vector<double> xs(TEST_NUM);
	std::vector<double> ys(TEST_NUM);
	for (size_t i = 0; i < xs.size(); i++)
	{
		xs[i] = rand() % 2048;
		ys[i] = rand() % 2048;
	}
	std::vector<double> result(TEST_NUM);

	TICK(t);
	for (int i = 0; i < TEST_NUM; i += WIDTH)
	{
		GetSubPixelValue(result.data() + i, preCaculatedParameter.data(), width, height, xs.data() + i, ys.data() + i);
	}
#ifdef __GNUC__
        asm volatile ("mov %0, %0" :: "g" (result.data()) : "cc", "memory");
#endif
	TOCK(t);

	return 0;
}
