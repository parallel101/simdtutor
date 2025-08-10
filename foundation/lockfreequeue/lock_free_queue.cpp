#include "affinity.h"
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <thread>
#include "mt_queue.h"
#include "spsc_ring_queue.h"
#include "show_time.h"


mt_queue<int> q(1024);
spsc_ring_queue<int, 1024> ring;


const int N = 1024 * 1024;

void producer()
{
    for (int i = 0; i < N; ++i) {
        q.push(i);
    }
}

void consumer()
{
    for (int i = 0; i < N; ++i) {
        int value = q.pop();
        if (value != i) {
            fmt::println("Data Error: {} != {}", value, i);
        }
    }
}

void ring_producer()
{
    auto w = ring.writer();
    for (int i = 0; i < N; ++i) {
        int value = i;
        while (w.write_n(&value, 1) != 1);
    }
}

void ring_consumer()
{
    auto r = ring.reader();
    for (int i = 0; i < N; ++i) {
        int value = -1;
        while (r.read_n(&value, 1) != 1);
        if (value != i) {
            fmt::println("Data Error: {} != {}", value, i);
            exit(1);
        }
    }
}

int main()
{
    for (int i = 0; i < 100; ++i) {
        show_time _("queue");
        std::jthread producer_thread(ring_producer);
        std::jthread consumer_thread(ring_consumer);
        producer_thread.join();
        consumer_thread.join();
    }
}
