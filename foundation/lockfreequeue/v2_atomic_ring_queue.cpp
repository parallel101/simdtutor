#include <fmt/format.h>
#include <fmt/ranges.h>
#include <thread>
#include "show_time.h"
#include <smmintrin.h>


int ring[1024];
std::atomic_int write_pos;
std::atomic_int read_pos;

void ring_push(int value)
{
again:
    int w = write_pos.load();
    int next_w = (w + 1) % 1024;
    if (next_w == read_pos.load()) {
        goto again;
    }
    ring[w] = value;
    write_pos.store(next_w);
}

int ring_pop()
{
again:
    int r = read_pos.load();
    if (r == write_pos.load()) {
        goto again;
    }
    int next_r = (r + 1) % 1024;
    int value = ring[r];
    read_pos.store(next_r);
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
    show_time _("queue");
    std::jthread producer_thread(producer);
    std::jthread consumer_thread(consumer);
    producer_thread.join();
    consumer_thread.join();
}
