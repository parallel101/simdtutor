#include <cmath>
#include <immintrin.h>

// 以下是项目中的一段热点代码
// 会调用这个函数很多次，用来计算亚像素的像素值，方法是插值（具体算法可以不用管），里头的具体魔数我改了一下，因为是公司的代码，而且跟优化没有关系

double GetSubPixelValue(const double* preCaculatedParameter, int width, int height, double X, double Y)
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
		xWeight[0] = 0.111 * w4 - w3 * (1.0 / 22.0) + 0.233 * w2 - 0.344 * w + 0.345;
		xWeight[1] = -0.333 * w4 + 0.333 * w3 + 0.444 * w2 - 0.555 * w + 0.0666;
		xWeight[2] = 0.15 *w4 - 0.222 * w2 + 0.0777;
		xWeight[3] = -0.333 * w4 - 0.333 * w3 + 0.444 * w2 + 0.555 * w + 0.0666;
		xWeight[4] = 0.111 * w4 + w3 * (1.0 / 22.0) + 0.233 * w2 + 0.344 * w + 0.345;
	}

	// 下面这个计算方法和上面是一样的
	double yWeight[5];
	{
		double w = Y - (double)yIndex[2];
		double w2 = w * w;
		double w3 = w * w2;
		double w4 = w2 * w2;
		yWeight[0] = 0.111 * w4 - w3 * (1.0 / 22.0) + 0.233 * w2 - 0.344 * w + 0.345;
		yWeight[1] = -0.333 * w4 + 0.333 * w3 + 0.444 * w2 - 0.555 * w + 0.0666;
		yWeight[2] = 0.15 *w4 - 0.222 * w2 + 0.0777;
		yWeight[3] = -0.333 * w4 - 0.333 * w3 + 0.444 * w2 + 0.555 * w + 0.0666;
		yWeight[4] = 0.111 * w4 + w3 * (1.0 / 22.0) + 0.233 * w2 + 0.344 * w + 0.345;
	}

	int width2 = 2 * width - 2;
	int height2 = 2 * height - 2;
	for (int i = 0; i < 5; i++)
	{
		xIndex[i] = abs(xIndex[i]);
		xIndex[i] = xIndex[i] - width2 * (xIndex[i] / width2);
        xIndex[i] = width <= xIndex[i] ? width2 - xIndex[i] : xIndex[i];

		yIndex[i] = abs(yIndex[i]);
		yIndex[i] = yIndex[i] - height2 * (yIndex[i] / height2);
		yIndex[i] = height <= yIndex[i] ? height2 - yIndex[i] : yIndex[i];
        yIndex[i] = yIndex[i] * width;
	}

	double result = 0.0;
	for (int i = 0; i < 5; i++)
	{
        const double * rowPointer = preCaculatedParameter + yIndex[i];
        double rowResult = 0.0;
		for (int j = 0; j < 5; j++)
		{
			result += rowPointer[xIndex[j]] * xWeight[j];
		}
        result += rowResult * yWeight[i];
	}
	return result;
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
	std::vector<std::array<double, 2>> xys(TEST_NUM);
	for (size_t i = 0; i < xys.size(); i++)
	{
		xys[i] = {(double)(rand() % 2048), (double)(rand() % 2048)};
	}
    /* std::sort(xys.begin(), xys.end()); */
	std::vector<double> result(TEST_NUM);

	TICK(t);
    /* #pragma omp parallel for */
	for (int i = 0; i < TEST_NUM; i++)
	{
		result[i] = GetSubPixelValue(preCaculatedParameter.data(), width, height, xys[i][0], xys[i][1]);
	}
#ifdef __GNUC__
        asm volatile ("movq %0, %%rax" :: "g" (result.data()) : "cc", "memory");
#endif
	TOCK(t);

	return 0;
}
