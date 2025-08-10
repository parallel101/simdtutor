#include <fmt/format.h>
#include <fmt/ranges.h>
#include <thread>
#include "mt_queue.h"
#include "show_time.h"


mt_queue<int> q(1024);


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
