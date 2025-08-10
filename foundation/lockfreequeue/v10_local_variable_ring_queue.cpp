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

void ring_push(const int *buf, const int *end)
{
    int *write_pos_local = ::write_pos_local;
    int *read_pos_cached = ::read_pos_cached;
    while (buf != end) {
        int *next_write_pos = write_pos_local;
        ++next_write_pos;
        if (next_write_pos == ring + 1024) {
            next_write_pos = ring;
        }
        if (next_write_pos == read_pos_cached) {
            write_pos.store(write_pos_local, std::memory_order_release);
            do read_pos_cached = read_pos.load(std::memory_order_acquire);
            while (next_write_pos == read_pos_cached);
        }
        *write_pos_local = *buf++;
        write_pos_local = next_write_pos;
    }
    write_pos.store(write_pos_local, std::memory_order_release);
    ::write_pos_local = write_pos_local;
    ::read_pos_cached = read_pos_cached;
}

void ring_pop(int *buf, int *end)
{
    int *read_pos_local = ::read_pos_local;
    int *write_pos_cached = ::write_pos_cached;
    while (buf != end) {
        if (read_pos_local == write_pos_cached) {
            read_pos.store(read_pos_local, std::memory_order_release);
            do write_pos_cached = write_pos.load(std::memory_order_acquire);
            while (read_pos_local == write_pos_cached);
        }
        *buf++ = *read_pos_local;
        ++read_pos_local;
        if (read_pos_local == ring + 1024) {
            read_pos_local = ring;
        }
    }
    read_pos.store(read_pos_local, std::memory_order_release);
    ::read_pos_local = read_pos_local;
    ::write_pos_cached = write_pos_cached;
}


const int N = 1024 * 1024;

void producer()
{
    const int B = 512;
    static int buf[B];
    for (int i = 0; i < B; ++i) {
        buf[i] = i;
    }
    for (int j = 0; j < N / B; ++j) {
        ring_push(buf, buf + B);
    }
}

void consumer()
{
    const int B = 512;
    static int buf[B];
    for (int j = 0; j < N / B; ++j) {
        ring_pop(buf, buf + B);
        for (int i = 0; i < B; ++i) {
            if (buf[i] != i) [[unlikely]] {
                fmt::println("Data Error: {} != {}", buf[i], i);
                exit(1);
            }
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
