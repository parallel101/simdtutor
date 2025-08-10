#include <fmt/format.h>
#include <fmt/ranges.h>
#include <thread>
#include "show_time.h"
#include <smmintrin.h>


alignas(64) int ring[1024];
alignas(64) std::atomic<int *> write_pos{ring};
alignas(64) std::atomic<int *> read_pos{ring};
alignas(64) int *read_pos_cached = ring;
alignas(64) int *write_pos_cached = ring;
alignas(64) int *write_pos_local = ring;
alignas(64) int *read_pos_local = ring;

void ring_push(int value)
{
    int *w = write_pos_local;
    int *next_w = w + 1;
    if (next_w == ring + 1024) {
        next_w = ring;
    }
    if (next_w == read_pos_cached) {
        do read_pos_cached = read_pos.load(std::memory_order_acquire);
        while (next_w == read_pos_cached);
    }
    *w = value;
    write_pos_local = next_w;
    write_pos.store(next_w, std::memory_order_release);
}

int ring_pop()
{
again:
    int *r = read_pos_local;
    if (r == write_pos_cached) {
        do write_pos_cached = write_pos.load(std::memory_order_acquire);
        while (r == write_pos_cached);
    }
    int *next_r = r + 1;
    if (next_r == ring + 1024) {
        next_r = ring;
    }
    int value = *r;
    read_pos_local = next_r;
    read_pos.store(next_r, std::memory_order_release);
    return value;
}


const int N = 1024 * 1024;

void producer()
{
    for (int i = 0; i < N; ++i) {
        ring_push(i);
    }
}

void consumer()
{
    for (int i = 0; i < N; ++i) {
        int value = ring_pop();
        if (value != i) [[unlikely]] {
            fmt::println("Data Error: {} != {}", value, i);
            exit(1);
        }
    }
}

int main()
{
    for (int i = 0; i < 100; ++i) {
        show_time _("queue");
        std::jthread producer_thread(producer);
        std::jthread consumer_thread(consumer);
        producer_thread.join();
        consumer_thread.join();
    }
}
