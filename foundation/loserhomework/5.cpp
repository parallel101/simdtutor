#include <format>
#include <vector>
#include <functional>
#include <stdexcept>
#include <iostream>
#include <string>

using namespace std;

template <class F, class ...Args>
struct scope_guard { // scope_guard<F, int, double>
    F f;
    std::tuple<Args...> args;
    scope_guard(F f_, Args ...args_) : f(std::move(f_)), args(std::move(args_)...) {
    }
    ~scope_guard() {
        std::apply(std::move(f), std::move(args)); // std::invoke(f, get<0>(args), get<1>(args), ...);
    }
};

template <class F, class ...Args>
scope_guard(F f, Args ...args) -> scope_guard<F, Args...>;

int xmain() {
    puts("进入 main");
    {
        puts("进入局部");
        puts("打开文件");
        FILE *fp = fopen("CMakeLists.txt", "r");
        printf("打开的文件是：%p\n", fp);
        struct Test {
            void f(FILE *fp, int &i) const {
                puts("Test被调用了！");
                printf("关闭文件：%p %d\n", fp, i);
                fclose(fp);
            }
        };
        int i = 42;
        /* auto membf = &Test::f; */
        /* auto membv = &Test::mb; */
        /* Test t; */
        /* (t.*membf)(fp, i); */
        // void (Test::*)(FILE *, int &)
        // void (*)(FILE *, int &)
        scope_guard a{&Test::f, Test{}, fp, std::ref(i)};
        throw std::runtime_error("自定义异常信息");
        if (1) {
            puts("提前返回");
            return 2;
        }
        puts("离开局部");
    } // 析构
    puts("离开 main");
    return 1;
}

int main() {
    try {
        xmain();
    } catch (std::exception const &e) {
        puts(e.what());
    }
}
