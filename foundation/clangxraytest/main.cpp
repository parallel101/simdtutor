#include <cstdio>
#include <thread>

using namespace std::chrono_literals;

void hello()
{
    printf("Hello, world\n");
    std::this_thread::sleep_for(3ms);
}

int main()
{
    hello();
    std::this_thread::sleep_for(1ms);
    hello();
    std::this_thread::sleep_for(2ms);
}
