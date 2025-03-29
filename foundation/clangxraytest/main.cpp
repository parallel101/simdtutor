#include <cstdio>
#include <thread>

using namespace std::chrono_literals;

void hello()
{
    printf("Hello, world\n");
    std::this_thread::sleep_for(0.3s);
}

int main()
{
    hello();
    std::this_thread::sleep_for(0.1s);
    hello();
    std::this_thread::sleep_for(0.2s);
}
