#include <iostream>
#include <chrono>

#define TICK(x) auto bench_##x = std::chrono::steady_clock::now();
#define TOCK(x) std::cerr<<#x ": "<<std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now()-bench_##x).count();std::cerr<<"秒\n";

#include <tbb/parallel_for.h>
#include <tbb/concurrent_vector.h>
#include <tbb/enumerable_thread_specific.h>
#include <immintrin.h>
#include <memory_resource>
#include <random>

template <class T>
struct NoInit {
  T value;
  NoInit() {}  // 不是 = default，也不写 : value()，这样一来只要 T 是 POD 类型，value 就不会0初始化
  NoInit(T value_) : value(value_) {}  // 强制初始化的版本（T隐式转换为NoInit<T>）
  operator T const &() const { return value; } // NoInit<T>隐式转换为T
  operator T &() { return value; } // NoInit<T>隐式转换为T
};

__m256i masklut[512];

static int _init_lut = [] {
    for (int i = 0; i < 256; i++) {
        int per[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        for (int j = 0, k = 0; k < 8; k++) {
            if (i & (1 << k)) {
                per[j++] = k;
            }
        }
        __m256i perm = _mm256_loadu_si256((const __m256i *)per);
        _mm256_store_si256(masklut + i * 2, perm);
        int c = _mm_popcnt_u32(i);
        __m256i mask = _mm256_setr_epi32(
            c > 0 ? -1 : 0,
            c > 1 ? -1 : 0,
            c > 2 ? -1 : 0,
            c > 3 ? -1 : 0,
            c > 4 ? -1 : 0,
            c > 5 ? -1 : 0,
            c > 6 ? -1 : 0,
            c > 7 ? -1 : 0);
        _mm256_store_si256(masklut + i * 2 + 1, mask);
    }
    return 0;
} ();

// BEGIN CODE
template <int cmp>
size_t filterp(float const *x, size_t n, float y, float *z) {
    __m256 pred = _mm256_set1_ps(y);
    auto zbeg = z;
    auto xend = x + n;
    while (x + 16 <= xend) {
        __m256 xi = _mm256_loadu_ps(x);
        __m256 mask = _mm256_cmp_ps(xi, pred, cmp);
        __m256 xi2 = _mm256_loadu_ps(x + 8);
        x += 16;
        __m256 mask2 = _mm256_cmp_ps(xi2, pred, cmp);
        size_t m = (size_t)_mm256_movemask_ps(mask) << 6;
        size_t m2 = (size_t)_mm256_movemask_ps(mask2) << 6;
        const __m256i *mp = masklut + (m >> 5);
        __m256i wa = _mm256_load_si256(mp);
        __m256i wb = _mm256_load_si256(mp + 1);
        xi = _mm256_permutevar8x32_ps(xi, wa);
        _mm256_maskstore_ps(z, wb, xi);
        z += _mm_popcnt_u32((unsigned)m);
        mp = masklut + (m2 >> 5);
        wa = _mm256_load_si256(mp);
        wb = _mm256_load_si256(mp + 1);
        xi2 = _mm256_permutevar8x32_ps(xi2, wa);
        _mm256_maskstore_ps(z, wb, xi2);
        z += _mm_popcnt_u32((unsigned)m2);
    }
    for (; x < xend; x++) {
        __m128 xi = _mm_load_ss(x);
        __m128 mask = _mm_cmpgt_ss(xi, _mm_set_ss(y));
        int m = _mm_extract_ps(mask, 0);
        if (m) {
            _mm_store_ss(z++, xi);
        }
    }
    return z - zbeg;
}

// newdelete < sync < unsync < monot

int main() {
    std::vector<float> scores(65536 * 4096);
    // 随机填充学生成绩数据（0～100）
    tbb::parallel_for(
        tbb::blocked_range<size_t>(0, scores.size()),
        [&] (tbb::blocked_range<size_t> r) {
            std::mt19937 rng(r.begin());
            std::uniform_real_distribution<float> uni(0, 100);
            for (size_t i = r.begin(); i != r.end(); ++i) {
                scores[i] = uni(rng);
            }
        });
    // 开始过滤出 60 分以下的学生，进行批评教育
    TICK(filter);
    tbb::concurrent_vector<float> bad_scores;
    tbb::enumerable_thread_specific<std::pmr::unsynchronized_pool_resource> pool_ets;
    tbb::parallel_for(
        tbb::blocked_range<size_t>(0, scores.size(), 65536 * 32),
        [&] (tbb::blocked_range<size_t> r) {
            #if 1
            auto &pool = pool_ets.local();
            std::pmr::vector<NoInit<float>> local_bad_scores{&pool};
            #else
            std::vector<NoInit<float>> local_bad_scores;
            #endif
            local_bad_scores.resize(r.size());
            #if 1
            size_t n = filterp<_CMP_LT_OQ>((float *)scores.data() + r.begin(), r.size(), 60, (float *)local_bad_scores.data());
            #else
            size_t n = 0;
            for (size_t i = r.begin(); i != r.end(); ++i) {
                float score = scores[i];
                if (score < 60) {
                    local_bad_scores[n++] = score;
                }
            }
            #endif
            local_bad_scores.resize(n);
            std::copy(local_bad_scores.begin(), local_bad_scores.end(),
                      bad_scores.grow_by(local_bad_scores.size()));
        });
    TOCK(filter);
}
