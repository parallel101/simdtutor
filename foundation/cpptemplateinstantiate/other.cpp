#include "other.h"
#include <vector>
#include <iostream>
#include <tuple>
#include <algorithm>

template <class V>
struct variant_to_tuple_of_vector;

template <class ...Ts>
struct variant_to_tuple_of_vector<variant<Ts...>> {
    using type = tuple<vector<Ts>...>;
};

static variant_to_tuple_of_vector<Object>::type objects;

#if 0
template <size_t N, class Lambda>
void static_for(Lambda &&lambda) {
    if constexpr (N > 0) {
        static_for<N - 1>(lambda);
        lambda(integral_constant<size_t, N - 1>{});
    }
}
#else
template <size_t N, class Lambda>
auto static_for(Lambda &&lambda) {
    return [&] <size_t ...Is> (index_sequence<Is...>) {
        return (lambda(integral_constant<size_t, Is>{}), ...);
    }(make_index_sequence<N>{});
}

template <size_t N, class Lambda>
auto static_for_break_if_false(Lambda &&lambda) {
    return [&] <size_t ...Is> (index_sequence<Is...>) {
        return (lambda(integral_constant<size_t, Is>{}) && ...);
    }(make_index_sequence<N>{});
}

template <size_t N, class Lambda>
auto static_for_break_if_true(Lambda &&lambda) {
    return [&] <size_t ...Is> (index_sequence<Is...>) {
        return (lambda(integral_constant<size_t, Is>{}) || ...);
    }(make_index_sequence<N>{});
}
#endif

void add_object(Object o) {
    /* [&] <size_t ...Is> (index_sequence<Is...>) { */
    /*     return ((o.index() == Is && (get<Is>(objects).push_back(get<Is>(o)), false)) && ...); */
    /* }(make_index_sequence<variant_size_v<Object>>{}); */
    static_for_break_if_false<variant_size_v<Object>>([&] (auto ic) {
        if (o.index() == ic) {
            get<ic>(objects).push_back(get<ic>(o));
            return false;
        }
        return true;
    });
}

void print_objects() {
    /* [&] <size_t ...Is> (index_sequence<Is...>) { */
    /*     return (ranges::for_each(get<Is>(objects), [&] (auto o) { */
    /*         return (cout << o << endl, void()); */
    /*     }), ...); */
    /* }(make_index_sequence<variant_size_v<Object>>{}); */
    static_for<variant_size_v<Object>>([&] (auto ic) {
        for (auto const &o: get<ic>(objects)) {
            cout << o << endl;
        }
    });
}
