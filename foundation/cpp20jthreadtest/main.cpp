#include <cstdio>
#include <chrono>
#include <thread>
#include <atomic>
#include <vector>

using namespace std;

void worker(stop_token stoken, int id) {
    for (int i = 0; i < 60; i++) {
        if (stoken.stop_requested()) {
            printf("Thread %d canceled\n", id);
            std::this_thread::sleep_for(300ms);
            return;
        }
        std::this_thread::sleep_for(100ms);
    }
    printf("Thread %d exited\n", id);
}

int main() {
    vector<jthread> ts;
    for (int i = 0; i < 4; i++) {
        ts.emplace_back(worker, i);
    }
    puts("main");
    std::this_thread::sleep_for(1s);
    puts("canceling");
    ts[0].request_stop();
    ts[0].join();
    puts("canceled");
    ts[1].join();
    puts("joined");
    return 0;
}
