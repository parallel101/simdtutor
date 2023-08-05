#include <cstdio>
#include <cstdint>
#include <cstring>
#include <type_traits>
#include <immintrin.h>
#include <stdexcept>

template <int Offset>
static void memcpy_streamed(char *dst, char const *src, char const *nextSrc, size_t size) {
    const size_t width = 64;
    /* auto chDst = (char *)dst; */
    /* auto chSrc = (char const *)src; */
    /* auto chDstTemp = (char *)(uintptr_t)chDst */
    /* auto chDstEnd = (char *)dst + size; */
    /* while (size-- % width) { */
    /*     *chDst++ = *chSrc++; */
    /* } */
    // 如果 Numpy 的数组首地址离对齐到 64 字节差 16 字节
    // 为了对齐到缓存行以便使用 stream 和 prefetch，需要先把前面 3 个 16 字节当作边角料处理掉
    if (Offset) {
        for (int i = 0; i < 4-Offset; i++) {
            _mm_stream_si128((__m128i *)dst, _mm_loadu_si128((__m128i *)src));
            dst += 16;
            src += 16;
        }
        size -= Offset * 16;
    }
    /* if ((uintptr_t)dst % 64) [[unlikely]] { */
    /*     printf("%d, dst: %zd\n", Offset, (uintptr_t)dst % 64); */
    /*     throw std::runtime_error("not aligned dst"); */
    /* } */
    int batch = size / width;
    auto vecDst = (__m256i *)dst;
    auto vecSrc = (__m256i const *)src;
    auto vecDstEnd = vecDst + batch * width / sizeof(__m256i);
    while (vecDst != vecDstEnd - 2) {
        auto v1 = _mm256_loadu_si256(vecSrc);
        auto v2 = _mm256_loadu_si256(vecSrc + 1);
        _mm256_stream_si256(vecDst, v1);
        _mm256_stream_si256(vecDst + 1, v2);
        vecSrc += 2;
        vecDst += 2;
    }
        auto v2 = _mm256_loadu_si256(vecSrc + 1); // 故意反过来破坏当前预取序列
        auto v1 = _mm256_loadu_si256(vecSrc);
        _mm_prefetch(nextSrc, _MM_HINT_T0); // 告诉他接下来该处理下一行了
        _mm256_stream_si256(vecDst + 1, v2);
        _mm256_stream_si256(vecDst, v1);
        vecSrc += 2;
        vecDst += 2;
    // 还需要也把后面 1 个 16 字节当作边角料处理掉
    dst = (char *)vecDst;
    src = (char const *)vecSrc;
    if (Offset) {
        for (int i = 0; i < Offset; i++) {
            _mm_stream_si128((__m128i *)dst, _mm_loadu_si128((__m128i *)src));
            dst += 16;
            src += 16;
        }
    }
    _mm_prefetch(nextSrc + width, _MM_HINT_T0);
}

template<typename T>
void concat(T *source, T *destination, const size_t DIM_SRC_N,
            const size_t DIM_SRC_X, const size_t DIM_SRC_Y, const size_t DIM_SRC_Z,
            const size_t DIM_DST_X, const size_t DIM_DST_Y, const size_t DIM_DST_Z,
            const size_t START_X,   const size_t START_Y,   const size_t START_Z
            )
{
    // 开启 openmp 多线程加速, 好像2个线程就饱和了 // 小彭老师：我靠，memcpy是典型的内存瓶颈（membound）操作，当然没法用并行加速了
    // #pragma omp parallel for num_threads(2)
    // T (*vectorSRC)[DIM_SRC_Y][DIM_SRC_Z] = reinterpret_cast<T(*)[DIM_SRC_Y][DIM_SRC_Z]>(source);
    // T (*vectorDST)[DIM_DST_Y][DIM_DST_Z] = reinterpret_cast<T(*)[DIM_DST_Y][DIM_DST_Z]>(destination);

    size_t size_cp = DIM_SRC_N * DIM_DST_Z * sizeof(T);
    if (size_cp % 64) throw std::runtime_error("cannot handle bianjiaoliao for now");
    // 实测发现 Numpy 的数组首地址总是对齐到 16 字节
    if ((uintptr_t)destination % 16) { printf("%zd\n", (uintptr_t)destination % 64); throw std::runtime_error("dst must be 16B-aligned"); }

    auto threadconcat = [&] (auto offset) {
        #pragma omp parallel for num_threads(4)
        for (size_t ix = 0; ix < DIM_DST_X; ix++){
            for (size_t iy = 0; iy < DIM_DST_Y; iy++){
                // T * ptrSRC = &vectorSRC[ix][iy][0];
                // T * ptrDST = &vectorDST[ix+START_X][iy+START_Y][START_Z];
                T * ptrSRC = &source[DIM_SRC_N * ((ix + START_X) * DIM_SRC_Y * DIM_SRC_Z + (iy + START_Y) * DIM_SRC_Z + START_Z)];
                T * ptrNextSRC = &source[DIM_SRC_N * (((ix + (iy + 1) / DIM_DST_Y) + START_X) * DIM_SRC_Y * DIM_SRC_Z + ((iy + 1) % DIM_DST_Y + START_Y) * DIM_SRC_Z + START_Z)];
                T * ptrDST = &destination[DIM_SRC_N * (ix * DIM_DST_Y * DIM_DST_Z + iy * DIM_DST_Z)];
    /* printf("!!!pd%zd\n", (uintptr_t)ptrDST%64); */
                memcpy_streamed<offset.value>((char *)ptrDST, (char *)ptrSRC, (char *)ptrNextSRC, size_cp);
                // for (int iz = 0; iz < DIM_DST_Z; iz++){
                //     ptrDST[iz] = ptrSRC[iz];
                // }
            }
        }
    };

    int offset = (uintptr_t)destination % 64 / 16;
    /* printf("!!!of%d\n", offset); */
    switch (offset) {
    case 0: threadconcat(std::integral_constant<int, 0>()); break;
    case 1: threadconcat(std::integral_constant<int, 1>()); break;
    case 2: threadconcat(std::integral_constant<int, 2>()); break;
    case 3: threadconcat(std::integral_constant<int, 3>()); break;
    };
}
