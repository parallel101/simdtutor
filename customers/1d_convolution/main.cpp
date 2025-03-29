#include <cstring>
#include <random>
#include <chrono>
#include <vector>
#include <x86intrin.h>

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(1.0, 10.0);

    // 生成vec数据；
    size_t size1 = 10000;
    size_t size2 = 126;
    std::vector<float> vec(size1 * size2, 0);
    for (auto &x : vec) {
        x = dis(gen);
    }

    // 生成conv数据;
    size_t sizeb = 64;
    std::vector<float> b(sizeb);
    for (auto &x : b) {
        x = dis(gen);
    };

    // 临时向量，把vec中的每个向量存储到padZeroData中（前后各有一部分0）;
    std::vector<float> padZeroData(size2 + sizeb);

    // 分配结果向量；
    std::vector<float> result(size1 * size2, 0);

    auto t0 = std::chrono::steady_clock::now();
    for (int step = 0; step < 100; ++step) {
        // 开始计算。对于每个向量：
        for (int idx = 0; idx < size1; idx++) {

            // 将数据插入到padZeroData中；
            memcpy(&padZeroData[sizeb / 2], &vec[idx * size2], size2 * sizeof(float));

            // padZeroData和b两个向量卷积，结果放入vec中；
            for (int idy = 0; idy < size2; idy += 16) {
#define UNROLL_16 \
    UNROLL(0) UNROLL(1) UNROLL(2) UNROLL(3) \
    UNROLL(4) UNROLL(5) UNROLL(6) UNROLL(7) \
    UNROLL(8) UNROLL(9) UNROLL(10) UNROLL(11) \
    UNROLL(12) UNROLL(13) UNROLL(14) UNROLL(15)
#define UNROLL(offy) __m512 tmp##offy = _mm512_setzero_ps();
                UNROLL_16
#undef UNROLL
                for (int idz = 0; idz < sizeb; idz += 16) {
                    __m512 btmp = _mm512_loadu_ps(&b[idz]);
                    __m512i pad1 = _mm512_castps_si512(_mm512_loadu_ps(&padZeroData[idy + idz]));
                    __m512i pad2 = _mm512_castps_si512(_mm512_loadu_ps(&padZeroData[idy + 16 + idz]));
#define UNROLL(offy) tmp##offy = _mm512_fmadd_ps(_mm512_castsi512_ps(_mm512_alignr_epi32(pad2, pad1, offy)), btmp, tmp##offy);
                    UNROLL_16
#undef UNROLL
                }
                float *res = &result[idx * size2 + idy];
#define UNROLL(offy) res[offy] = _mm512_reduce_add_ps(tmp##offy);
                    UNROLL_16
#undef UNROLL
            }
        }
    }
    auto t1 = std::chrono::steady_clock::now();
    printf("%ld us\n", duration_cast<std::chrono::microseconds>(t1 - t0).count() / 100);
    return 0;
}
