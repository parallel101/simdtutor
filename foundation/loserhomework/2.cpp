#include <format>
#include <vector>
#include <functional>
#include <iostream>
#include <string>

using namespace std;

auto operator""_f(const char *s, size_t len) {
    auto fmt = string(s, len);
    return [=] (auto &&...args) {
        return vformat(fmt, make_format_args(args...));
    };
}

int main(){
    std::cout << "乐 :{} *\n"_f(5);
    std::cout << "乐 :{0} {0} *\n"_f(5);
    std::cout << "乐 :{:b} *\n"_f(0b01010101);
    std::cout << "{:*<10}"_f("卢瑟");
    std::cout << '\n';
    int n{};
    std::cin >> n;
    std::cout << "π：{:.{}f}\n"_f(std::numbers::pi_v<double>, n);
    return 1;
}
