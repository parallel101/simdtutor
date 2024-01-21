#include <format>
#include <vector>
#include <functional>
#include <iostream>
#include <string>

using namespace std;

struct CompBase {
    inline static size_t id_count = 0;
};

template <class T>
struct Comp : CompBase {
    inline static const size_t id = id_count++;
    static constexpr size_t component_type_id() {
        return id;
    }
};

struct A : Comp<A> {};
struct B : Comp<B> {};
struct C : Comp<C> {};

int main() {
    printf("A: %zd\n", A::component_type_id());
    printf("B: %zd\n", B::component_type_id());
    printf("C: %zd\n", C::component_type_id());
    printf("A: %zd\n", A::component_type_id());
    printf("A: %zd\n", A::component_type_id());
    printf("A: %zd\n", A::component_type_id());
    printf("C: %zd\n", C::component_type_id());
    return 1;
}
