#include <algorithm>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <random>
#include <utility>
#include <vector>
#include <omp.h>
#include "show_time.h"


// 1234 % 10 = 4
// 1234 / 10 % 10 = 3
// 1234 / 100 % 10 = 2
// 1234 / 1000 % 10 = 1

// 1234 % 100 = 34
// 1234 / 100 % 100 = 12

// 41 40 14 21
// 40 41 21 14: 1 pass bin sort bin=10
// 14 21 40 41
// bin=100 -> bin=10 bin=10
// 10^k -> 10
// n -> kn

void radix_sort_v1(uint32_t *a, size_t n) // 初版
{
    // merge_sort, quick_sort, bubble_sort, insert_sort: 基于比较的排序 O(n log n)
    // bin_sort, radix_sort: 是个整数（有上限的）类型 O(n)

    // 1 1 3 0 5
    // [0] [1 1] [] [3] [5]
    // 0 1 1 3 5

    std::vector<std::vector<uint32_t>> bin(256); // JavaBean (病)
    // m = [[1 2] [3 4] [5 6]]
    // a = [1 2 3 4 5 6]
    // a[i + 2 * j] == m[i][j]

    for (size_t p = 0; p < 4; ++p) {
        for (size_t b = 0; b < bin.size(); ++b) {
            bin[b].clear();
        }

        uint32_t shift = 8 * p;
        for (size_t i = 0; i < n; ++i) {
            uint32_t x = a[i];
            bin[(x >> shift) & 0xff].push_back(x);
        }

        size_t i = 0;
        for (size_t b = 0; b < bin.size(); ++b) {
            for (size_t c = 0; c < bin[b].size(); ++c) {
                a[i++] = bin[b][c];
            }
        }
    }
}

void radix_sort_v2(uint32_t *a, size_t n) // 提前大小统计 优化版
{
    std::vector<std::vector<uint32_t>> bin(256);
    std::vector<size_t> count(256); // 统计，接下来，每个桶，会有多少数据进来
    std::vector<size_t> offset(256); // 保存了，当前每个桶，已经填入了多少数据
    // offset[i] = [0, count[i])

    // bin = [[0] [1 1 1] [] [3] [5]]
    // count = [1 3 0 1 1]
    // range = [0 1=1 1+3=4 1+3+0=4 1+3+0+1=5 1+3+0+1+1=6]
    // bin = [0|1 1 1||3|5]

    // sum = reduce
    // prefix sum = scan
    // radix sort = count + scan + reorder

    // [0 1 2 3 4 . . .]

    for (size_t p = 0; p < 4; ++p) {
        for (size_t b = 0; b < bin.size(); ++b) {
            bin[b].clear();
            count[b] = 0;
            offset[b] = 0;
        }

        uint32_t shift = 8 * p;
        for (size_t i = 0; i < n; ++i) {
            uint32_t x = a[i];
            size_t b = (x >> shift) & 0xff;
            ++count[b];
        }

        for (size_t b = 0; b < bin.size(); ++b) {
            bin[b].resize(count[b]);
        }

        for (size_t i = 0; i < n; ++i) {
            uint32_t x = a[i];
            size_t b = (x >> shift) & 0xff;
            bin[b][offset[b]++] = x;
        }

        size_t i = 0;
        for (size_t b = 0; b < bin.size(); ++b) {
            for (size_t c = 0; c < bin[b].size(); ++c) {
                a[i++] = bin[b][c];
            }
        }
    }
}

void radix_sort_v3(uint32_t *a, size_t n) // scan 大法拍平桶数组 优化版
{
    std::vector<uint32_t> bin(n);
    std::vector<size_t> count(256); // 统计，接下来，每个桶，会有多少数据进来
    std::vector<size_t> offset(256); // 保存了，当前每个桶，已经填入了多少数据
    // offset[i] = [0, count[i])

    // bin = [[0] [1 1 1] [] [3] [5]]
    // count = [1 3 0 1 1]
    // offset = [0 1=1 1+3=4 1+3+0=4 1+3+0+1=5 1+3+0+1+1=6]
    // bin = [|0|1 1 1||3|5]

    // sum = reduce
    // prefix sum = scan
    // radix sort = count + scan + reorder

    // [0 1 2 3 4 . . .]

    for (size_t p = 0; p < 4; ++p) {
        for (size_t b = 0; b < 256; ++b) {
            count[b] = 0;
            offset[b] = 0;
        }

        uint32_t shift = 8 * p;
        for (size_t i = 0; i < n; ++i) {
            uint32_t x = a[i];
            size_t b = (x >> shift) & 0xff;
            ++count[b];
        }
        // count = [1 3 0 1 1]

        size_t sum = 0;
        for (size_t b = 0; b < 256; ++b) {
            offset[b] = sum;
            sum += count[b];
        }
        // offset = [0 1 4 4 5]

        for (size_t i = 0; i < n; ++i) {
            uint32_t x = a[i];
            size_t b = (x >> shift) & 0xff;
            bin[offset[b]++] = x;
        }
        // offset = [1 3 4 5 6]
        // bin = [|0|1 1 1||4|5]

        for (size_t i = 0; i < n; ++i) {
            a[i] = bin[i];
        }
    }
}


void radix_sort_v4(uint32_t *a, size_t n) // 双缓冲 优化版
{
    std::vector<uint32_t> bin(n);
    std::vector<size_t> count(256); // 2KB
    std::vector<size_t> offset(256); // 2KB

    uint32_t *b = bin.data();

    for (size_t p = 0; p < 4; ++p) {
        for (size_t h = 0; h < 256; ++h) {
            count[h] = 0;
            offset[h] = 0;
        }

        uint32_t shift = 8 * p;
        for (size_t i = 0; i < n; ++i) {
            uint32_t x = a[i];
            size_t h = (x >> shift) & 0xff;
            ++count[h];
        }

        size_t sum = 0;
        for (size_t h = 0; h < 256; ++h) {
            offset[h] = sum;
            sum += count[h];
        }

        for (size_t i = 0; i < n; ++i) {
            uint32_t x = a[i];
            size_t h = (x >> shift) & 0xff;
            b[offset[h]++] = x;
        }

        std::swap(a, b); // double-buffer: 双缓冲（图形学中非常常用）
    }

    // pass 0: a -> bin
    // memcpy: bin -> a
    // pass 1: a -> bin
    // memcpy: bin -> a
    // pass 2: a -> bin
    // memcpy: bin -> a
    // pass 3: a -> bin
    // memcpy: bin -> a

    // pass 0: a -> bin
    // pass 1: bin -> a
    // pass 2: a -> bin
    // pass 3: bin -> a

    // a = ata
    // b = bin
    // pass 0: a -> b (ata -> bin)
    // swap(a, b)
    // pass 1: a -> b (bin -> ata)
    // swap(a, b)
    // pass 2: a -> b (ata -> bin)
    // swap(a, b)
    // pass 3: a -> b (bin -> ata)
}


void radix_sort_v5(uint32_t *a, size_t n) // 桶扩大 65536 优化版
{
    std::vector<uint32_t> bin(n);
    std::vector<uint32_t> count(65536); // 512KB
    std::vector<uint32_t> offset(65536); // 512KB

    uint32_t *b = bin.data();

    for (size_t p = 0; p < 2; ++p) {
        uint32_t shift = 16 * p;

        for (size_t h = 0; h < 65536; ++h) {
            count[h] = 0;
            offset[h] = 0;
        }

        for (size_t i = 0; i < n; ++i) {
            uint32_t x = a[i];
            uint32_t h = (x >> shift) & 0xffff;
            ++count[h];
        }

        size_t sum = 0;
        for (size_t h = 0; h < 65536; ++h) {
            offset[h] = sum;
            sum += count[h];
        }

        for (size_t i = 0; i < n; ++i) {
            uint32_t x = a[i];
            uint32_t h = (x >> shift) & 0xffff;
            b[offset[h]++] = x;
        }

        std::swap(a, b);
    }
}


void radix_sort_v6(uint32_t *a, size_t n) // 多线程 优化版
{
    size_t nproc = omp_get_num_procs();

    std::vector<uint32_t> bin(n);
    std::vector<uint32_t> count(65536); // 256KB
    std::vector<uint32_t> lcount(65536 * nproc); // 256KB * 16 = 4MB

    // 65536 | 65536 | 65536 | 65536 | 65536 | 65536 | 65536 | 65536 | 65536 | 65536 | 65536

    uint32_t *b = bin.data();

    for (size_t p = 0; p < 2; ++p) {
        uint32_t shift = 16 * p;

        for (size_t h = 0; h < 65536 * nproc; ++h) {
            lcount[h] = 0;
        }

        size_t chunk = (n + nproc - 1) / nproc;
        // 59 / 10 = 5         floor(5.9) = 5
        // (59 + 9) / 10 = 6   ceil(5.9)  = 6
        // (60 + 9) / 10 = 6   ceil(6.0)  = 6

        #pragma omp parallel for schedule(static, 1)
        for (size_t c = 0; c < nproc; ++c) {
            for (size_t i = c * chunk; i < n && i < (c + 1) * chunk; ++i) {
                uint32_t x = a[i];
                uint32_t h = (x >> shift) & 0xffff;
                ++lcount[65536 * c + h];
            }
        }

        for (size_t h = 0; h < 65536; ++h) {
            count[h] = 0;
            for (size_t c = 0; c < nproc; ++c) {
                count[h] += lcount[65536 * c + h];
            }
        }

        size_t sum = 0;
        for (size_t h = 0; h < 65536; ++h) {
            uint32_t c = count[h];
            count[h] = sum;
            sum += c;
        }

        for (size_t h = 0; h < 65536; ++h) {
            size_t sum = 0;
            for (size_t c = 0; c < nproc; ++c) {
                uint32_t x = lcount[65536 * c + h];
                lcount[65536 * c + h] = sum;
                sum += x;
            }
        }

        // count = [0 3 4] 6
        // bin = [|. ._.|._|._.|]

        #pragma omp parallel for schedule(static, 1)
        for (size_t c = 0; c < nproc; ++c) {
            for (size_t i = c * chunk; i < n && i < (c + 1) * chunk; ++i) {
                uint32_t x = a[i];
                uint32_t h = (x >> shift) & 0xffff;
                b[count[h] + lcount[65536 * c + h]++] = x;
            }
        }

        std::swap(a, b);
    }
}


void radix_sort_v7(uint32_t *a, size_t n) // 最终 优化版
{
    size_t nproc = omp_get_num_procs();

    std::vector<uint32_t> bin(n);
    std::vector<uint32_t> count(65536);
    std::vector<uint32_t> lcount(65536 * nproc);

    uint32_t *b = bin.data();

    for (size_t p = 0; p < 2; ++p) {
        uint32_t shift = 16 * p;

        size_t chunk = (n + nproc - 1) / nproc;
        #pragma omp parallel for schedule(static, 1)
        for (size_t c = 0; c < nproc; ++c) {
            uint32_t *lc = lcount.data() + 65536 * c;
            for (size_t h = 0; h < 65536; ++h) {
                lc[h] = 0;
            }
            for (size_t i = c * chunk; i < n && i < (c + 1) * chunk; ++i) {
                uint32_t x = a[i];
                uint32_t h = (x >> shift) & 0xffff;
                ++lc[h];
            }
        }

        size_t sum = 0;
        for (size_t h = 0; h < 65536; ++h) {
            uint32_t lsum = 0;
            for (size_t c = 0; c < nproc; ++c) {
                uint32_t x = lcount[c * 65536 + h];
                lcount[c * 65536 + h] = lsum;
                lsum += x;
            }
            count[h] = sum;
            sum += lsum;
        }

        // count = [0 3 4] 6
        // bin = [|. ._.|._|._.|]

        #pragma omp parallel for schedule(static, 1)
        for (size_t c = 0; c < nproc; ++c) {
            uint32_t *lc = lcount.data() + 65536 * c;
            for (size_t h = 0; h < 65536; ++h) {
                lc[h] += count[h];
            }
            for (size_t i = c * chunk, end = std::min(n, (c + 1) * chunk); i < end; ++i) {
                uint32_t x = a[i];
                uint32_t h = (x >> shift) & 0xffff;
                b[lc[h]++] = x;
            }
        }

        std::swap(a, b);
    }
}


void randomize_data(uint32_t *a, size_t n)
{
    std::generate(a, a + n, [rng = std::mt19937{}, uni = std::uniform_int_distribution<uint32_t>{0, UINT32_MAX}] () mutable { return uni(rng); });
}


int main()
{
    std::vector<uint32_t> ini(100000000); // 一千万
    randomize_data(ini.data(), ini.size());

    // {
    //     std::vector<uint32_t> a = ini;
    //     {
    //         show_time _("std::sort");
    //         std::sort(a.data(), a.data() + a.size());
    //     }
    //     if (!std::is_sorted(a.begin(), a.end())) throw;
    // }

    // {
    //     std::vector<uint32_t> a = ini;
    //     {
    //         show_time _("radix_sort_v1");
    //         radix_sort_v1(a.data(), a.size());
    //     }
    //     if (!std::is_sorted(a.begin(), a.end())) throw;
    // }
    //
    // {
    //     std::vector<uint32_t> a = ini;
    //     {
    //         show_time _("radix_sort_v2");
    //         radix_sort_v2(a.data(), a.size());
    //     }
    //     if (!std::is_sorted(a.begin(), a.end())) throw;
    // }
    //
    // {
    //     std::vector<uint32_t> a = ini;
    //     {
    //         show_time _("radix_sort_v3");
    //         radix_sort_v3(a.data(), a.size());
    //     }
    //     if (!std::is_sorted(a.begin(), a.end())) throw;
    // }
    //
    // {
    //     std::vector<uint32_t> a = ini;
    //     {
    //         show_time _("radix_sort_v4");
    //         radix_sort_v4(a.data(), a.size());
    //     }
    //     if (!std::is_sorted(a.begin(), a.end())) throw;
    // }
    //
    {
        std::vector<uint32_t> a = ini;
        {
            show_time _("radix_sort_v5");
            radix_sort_v5(a.data(), a.size());
        }
        if (!std::is_sorted(a.begin(), a.end())) throw;
    }

    {
        std::vector<uint32_t> a = ini;
        {
            show_time _("radix_sort_v6");
            radix_sort_v6(a.data(), a.size());
        }
        if (!std::is_sorted(a.begin(), a.end())) throw;
    }

    {
        std::vector<uint32_t> a = ini;
        {
            show_time _("radix_sort_v7");
            radix_sort_v7(a.data(), a.size());
        }
        if (!std::is_sorted(a.begin(), a.end())) throw;
    }
}
