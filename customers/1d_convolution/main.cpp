#include <random>
#include <chrono>
#include <vector>

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
    // 开始计算。对于每个向量：
    for (int idx = 0; idx < size1; idx++) {

        // 将数据插入到padZeroData中；
        for (int idy = 0; idy < size2; idy++) {
            padZeroData[idy + sizeb / 2] = vec[idx * size2 + idy];
        }

        // padZeroData和b两个向量卷积，结果放入vec中；
        for (int idy = 0; idy < size2; idy++) {
            for (int idz = 0; idz < sizeb; idz++) {
                result[idx * size2 + idy] += padZeroData[idy + idz] * b[idz];
            }
        }
    }
    auto t1 = std::chrono::steady_clock::now();
    printf("%ld ms\n", duration_cast<std::chrono::milliseconds>(t1 - t0).count());
    return 0;
}
