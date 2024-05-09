#include <thread>
#include <vector>
#include "debug.hpp"

// 读操作: a.size()
// 写操作: a.push_back(1)

std::vector<int> a = {7, 8, 9};

void worker1() {
    debug(), a[0];
}

void worker2() {
    debug(), a[0];
}

void worker3() {
    debug(), a[0];
}

int main() {
    std::jthread t1(worker1);
    std::jthread t2(worker2);
    std::jthread t3(worker3);
    t1.join();
    t2.join();
    t3.join();
    debug(), a;
}
