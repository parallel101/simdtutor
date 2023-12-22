#include <omp.h>
#include <chrono>
#include <iostream>
#include <vector>

#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#define checkCudaErrors(x) do { cudaError_t __ret = (x); if (__ret) { printf("CUDA ERROR %d: " #x "\n", __ret); abort(); } } while (0)

#define TYPE double
#define imgW 2448
#define imgH 2048
#define N (imgW*imgH)

__constant__ TYPE c_para0[] = {1.5, 1.5, 1.5, 1.5, 1.5, 1.5};
__constant__ TYPE c_para1[] = {1.5, 1.5, 1.5, 1.5, 1.5, 1.5};
__constant__ TYPE c_para2[] = {1246, 1037, 2448, 2048};

#if 1
__global__ void GPU_Cal(TYPE *input, TYPE *output, int width, int height, TYPE *para0, TYPE *para1,
                                      TYPE *para2) {
    // 2d grid stride loop
    for (int row = blockIdx.y * blockDim.y + threadIdx.y; row < height; row += blockDim.y * gridDim.y) {
        for (int col = blockIdx.x * blockDim.x + threadIdx.x; col < width; col += blockDim.x * gridDim.x) {
            int i = row * width + col;
            TYPE data = input[i];
            TYPE x = (row - para2[0]) * para2[2];
            TYPE y = (col - para2[1]) * para2[3];

            const TYPE a = para0[0] + para0[2] * x + data * (para0[1] + para0[3] * x) + para0[4] * y + data * para0[5] * y;
            const TYPE b = para1[0] + para1[2] * x + data * (para1[1] + para1[3] * x) + para1[4] * y + data * para1[5] * y;

            output[i] = a / b;
        }
    }
}
#else
__global__ void GPU_Cal(TYPE *input, TYPE *output, int width, int height, TYPE *para0, TYPE *para1,
                                      TYPE *para2) {
    for (int i = threadIdx.x + blockIdx.x * blockDim.x; i < width * height; i += gridDim.x * blockDim.x) {
        TYPE data = input[i];
        int row = i / width;
        int col = i % height;
        TYPE x = (col - para2[0]) * para2[2];
        TYPE y = (row - para2[1]) * para2[3];

        const TYPE a = para0[0] + para0[2] * x + data * (para0[1] + para0[3] * x) + para0[4] * y + data * para0[5] * y;
        const TYPE b = para1[0] + para1[2] * x + data * (para1[1] + para1[3] * x) + para1[4] * y + data * para1[5] * y;

        output[i] = a / b;
    }
}
#endif

void CPU_Cal(const TYPE *input, TYPE *output, int width, int height, TYPE *para0, TYPE *para1, TYPE *para2) {
#pragma omp parallel for
    for (int row = 0; row < height; ++row) {
        TYPE *_output = output + row * width;
        const TYPE *_input = input + row * width;
        for (int col = 0; col < width; ++col) {
            const TYPE data = *_input;
            const TYPE x = (col - para2[0]) * para2[2];
            const TYPE y = (row - para2[1]) * para2[3];

            const TYPE a =
                para0[0] + para0[2] * x + data * (para0[1] + para0[3] * x) + para0[4] * y + data * para0[5] * y;
            const TYPE b =
                para1[0] + para1[2] * x + data * (para1[1] + para1[3] * x) + para1[4] * y + data * para1[5] * y;

            *_output = a / b; 
            ++_output;
            ++_input;
        }
    }
}

int main() {
    // 准备数据
    std::vector<TYPE> input(N, 2);
    std::vector<TYPE> output(N, 0);
    std::vector<TYPE> para0(30, 1.5);
    std::vector<TYPE> para1(30, 1.5);
    std::vector<TYPE> para3{1246, 1037, 2448, 2048};
    // 随机准备一段数据
    for (int i = 0; i < N; ++i) {
        input[i] = (double)i / N;
        output[i] = (double)i / N + 2;
    }
    for (int i = 0; i < 30; ++i) {
        para0[i] = (double)i / 30;
        para1[i] = (double)i / 30 + 4.0;
    }

    TYPE *d_input;
    TYPE *d_output;
    TYPE *d_para0;
    TYPE *d_para1;
    TYPE *d_para2;
    cudaMalloc((void **)&d_input, N * sizeof(TYPE));
    cudaMalloc((void **)&d_output, N * sizeof(TYPE));
    cudaMalloc((void **)&d_para0, 30 * sizeof(TYPE));
    cudaMalloc((void **)&d_para1, 30 * sizeof(TYPE));
    cudaMalloc((void **)&d_para2, 4 * sizeof(TYPE));
    cudaMemcpy(d_input, input.data(), N * sizeof(TYPE), cudaMemcpyHostToDevice);
    cudaMemcpy(d_output, output.data(), N * sizeof(TYPE), cudaMemcpyHostToDevice);
    cudaMemcpy(d_para0, para0.data(), 30 * sizeof(TYPE), cudaMemcpyHostToDevice);
    cudaMemcpy(d_para1, para1.data(), 30 * sizeof(TYPE), cudaMemcpyHostToDevice);
    cudaMemcpy(d_para2, para3.data(), 4 * sizeof(TYPE), cudaMemcpyHostToDevice);

    // GPU计算时间（取最短时间）
    dim3 thread_num = dim3(32, 32, 1);
    dim3 block_num = dim3(256, 256, 1);
    double gpu_time = 10000000;
    checkCudaErrors(cudaDeviceSynchronize());
    for (size_t i = 0; i < 50; i++) {
        auto t0 = std::chrono::steady_clock::now();
        GPU_Cal<<<block_num, thread_num>>>(d_input, d_output, imgW, imgH, d_para0, d_para1, d_para2);
        checkCudaErrors(cudaDeviceSynchronize());
        double time =
            std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - t0).count();
        gpu_time = std::min(gpu_time, time);
    }
    std::cout << "GPU time: " << gpu_time << std::endl;

    // CPU计算时间（取最短时间）
    TYPE *h_output;
    h_output = (TYPE *)malloc(N * sizeof(TYPE));
    cudaMemcpy(h_output, d_output, N * sizeof(TYPE), cudaMemcpyDeviceToHost);
    double cpu_time = 10000000;
    for (size_t i = 0; i < 50; i++) {
        auto t0 = std::chrono::steady_clock::now();
        CPU_Cal(input.data(), output.data(), imgW, imgH, para0.data(), para1.data(), para3.data());
        double time =
            std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - t0).count();
        cpu_time = std::min(cpu_time, time);
    }
    std::cout << "CPU time: " << cpu_time << std::endl;
    std::cout << "ratio: " << cpu_time / gpu_time << std::endl;

    // 检测计算结果是否一致
    for (int i = 0; i < N; i++) {
        if (h_output[i] != h_output[i] && output[i] != output[i]) {
            continue;
        }
        if (fabs(h_output[i] - output[i]) > 1e-2) {
            printf("Error! i: %d, cpu: %f, gpu:%f.\n", i, output[i], h_output[i]);
            abort();
        }
    }

    cudaFree(d_input);
    cudaFree(d_output);
    cudaFree(d_para0);
    cudaFree(d_para1);
    cudaFree(d_para2);
    return 0;
}
