#include <concepts>
#include <format>
#include <vector>
#include <functional>
#include <stdexcept>
#include <iostream>
#include <string>
#include <atomic>


template<class Ty,size_t size>
struct array {
    Ty* begin() { return arr; };
    Ty* end() { return arr + size; };
    Ty arr[size];
};

/* template <class Arg0, class ...Args> requires (sizeof...(Args) == 0 || (std::same_as<Arg0, Args> && ...)) */
/* array(Arg0 arg0, Args...args) -> array<Arg0, sizeof...(Args) + 1>; */

template <class Arg0, class ...Args>
array(Arg0 arg0, Args...args) -> array<std::common_type_t<Arg0, Args...>, sizeof...(Args) + 1>;

/* template <class T> */
/* array(std::initializer_list<T> ilist) -> array<T, ilist.size()>; */
// initializer_list != initializer-list

int main() {
    ::array arr{1, 3.14, 2.718f};
    for (const auto& i : arr) {
        std::cout << i << ' ';
    }
}
