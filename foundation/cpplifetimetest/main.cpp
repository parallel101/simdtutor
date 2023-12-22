#include <cstdio>
#include <memory>
#include <string>
#include "cppdemangle.h"

struct Test {
    char buf[1024];
    std::string name;
    explicit Test(std::string name = "") : name(std::move(name)) {
        printf("Test() %s\n", name.c_str());
    }
    Test(Test &&that) noexcept : name(that.name) {
        // that -> this
        that.name = "null";
        name += "_move";
        printf("Test(Test &&) %s\n", name.c_str());
    }
    Test &operator=(Test &&that) noexcept {
        name += "_move";
        printf("Test &operator=(Test &&) %s\n", name.c_str());
        name = that.name;
        return *this;
    }
    Test(Test const &that) : name(that.name) {
        name += "_copy";
        printf("Test(Test const &) %s\n", name.c_str());
    }
    Test &operator=(Test const &that) {
        name = that.name;
        name += "_copy";
        printf("Test &operator=(Test const &) %s\n", name.c_str());
        return *this;
    }
    ~Test() noexcept {
        printf("~Test() %s\n", name.c_str());
    }
};

/* void func(Test t) { */
/*     puts("func"); */
/* } */
/*  */
/* void funcref(Test &t) { */
/*     puts("func"); */
/* } */

/* std::string const &func(std::string const &s) { */
/*     return s; */
/* } */

/* void func(std::unique_ptr<Test> t) { */
/*     puts("func"); */
/* } */

// 主类型 std::string
// 弱引用类型 std::string_view、const char *
// 主类型 std::vector<T>
// 弱引用类型 std::span<T>、(T *, size_t)
// 主类型 std::unique_ptr<T>
// 弱引用类型 T *
// 主类型 std::shared_ptr<T>
// 弱引用类型 std::weak_ptr<T>、T *

std::string split(std::string s, char c) {
    auto pos = s.find(c);
    if (pos == std::string::npos) {
        return s;
    }
    return s.substr(0, pos);
}

void func(Test &&t) {
    puts("func(Test &&t)");
}

void func(Test const &t) {
    puts("func(Test const &t)");
}

void func(int i, int j) {
    puts("func(int i, int j)");
}

#define FWD(x) std::forward<decltype(x)>(x)
template <typename ...T>
void func2(T &&...t) {
    func(FWD(t)...);
}

int main() {
    Test t("t");
    func2(std::move(t));
    func2(t);
    func2(3, 4);
    /* t = t2; */
    /* Test t2(std::move(t)); */
    /* t2 = std::move(t); */
    /* std::unique_ptr<Test> t(new Test("t")); */
    /* func(std::move(t)); */
    /* auto ret = func(std::string("hello")); */
    /* printf("%s\n", ret.c_str()); */
    /* Test t("t"); */
    /* func(std::move(t)), puts("==="); */
    /* { Test t_move(std::move(t)); funcref(t_move), puts("==="); } */
    /* puts("---"); */
    return 0;
}
