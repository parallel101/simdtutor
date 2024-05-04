#include <thread>
#include <vector>
#include "debug.hpp"

std::vector<int> a = {7, 8, 9};

void worker1() {
    a.push_back(1);
}

void worker2() {
    a.push_back(2);
}

int main() {
    std::jthread t1(worker1);
    std::jthread t2(worker2);
    t1.join();
    t2.join();
}
