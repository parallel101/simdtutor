#include <cstddef>

template <std::size_t N>
struct static_string {
    char str[N];
};

template <std::size_t N>
constexpr const char *impl_make_static_string(const char (&str)[N]) {
    return str;
}

template <std::size_t N>
constexpr auto make_static_string(const char (&str)[N]) {
    static_string<N> ret;
    for (std::size_t i = 0; i < N; ++i) {
        ret.str[i] = str[i];
    }
    return ret;
}

int main() {
    auto s = make_static_string("hello");
}
