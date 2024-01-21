#pragma once

#include <iostream>
#include <tuple>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <array>
#include <list>
#include <deque>
#include <type_traits>

struct printer {
    std::ostream &out;
    bool first = true;

    printer(): out(std::cout) {}
    printer(std::ostream &out): out(out) {}

    template <class T>
    printer &operator,(T const &t) {
        if (first) {
            first = false;
        } else {
            out << ' ';
        }
        out << t;
        return *this;
    }

    template <class ...Ts>
    printer &operator,(std::tuple<Ts...> const &t) {
        [&]<size_t ...Is>(std::index_sequence<Is...>) {
            (operator,(std::get<Is>(t)), ...);
        }(std::index_sequence_for<Ts...>{});
        return *this;
    }

    template <class T>
    printer &operator,(std::vector<T> const &t) {
        for (auto const &x: t) {
            operator,(x);
        }
        return *this;
    }

    template <class T>
    printer &operator,(std::list<T> const &t) {
        for (auto const &x: t) {
            operator,(x);
        }
        return *this;
    }

    template <class T>
    printer &operator,(std::deque<T> const &t) {
        for (auto const &x: t) {
            operator,(x);
        }
        return *this;
    }

    template <class T>
    printer &operator,(std::set<T> const &t) {
        for (auto const &x: t) {
            operator,(x);
        }
        return *this;
    }

    template <class T>
    printer &operator,(std::unordered_set<T> const &t) {
        for (auto const &x: t) {
            operator,(x);
        }
        return *this;
    }

    ~printer() {
        if (first) return;
        out << '\n';
    }
    
    printer(printer &&) = delete;
};
