#include <vector>
#include <functional>
#include <iostream>
#include <string>

using namespace std;

template <class T>
auto &operator|(vector<T> &vec, std::invocable<T &> auto const &func) {
    for (auto &element: vec) {
        func(element);
    }
    return vec;
}

int main(){
    std::vector vec{1, 2, 3};
    std::function f {[] (const int& i) {std::cout << i << ' '; } };
    auto f2 = [] (int& i) {i *= i; };
    vec | f2 | f;
}
