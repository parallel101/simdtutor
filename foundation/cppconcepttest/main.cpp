#include <concepts>
#include <cstdio>
#include <string>

void print(int a) {
    printf("int: %d\n", a);
}

void print(double a) {
    printf("double: %lf\n", a);
}

void print(const char *a) {
    printf("const char *: %s\n", a);
}

void print(std::string a) {
    printf("string: %s\n", a.c_str());
}

int main() {
    print(42);
    print("你好");
    print(std::string("你好"));
    return 0;
}
