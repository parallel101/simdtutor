#include <llvm/ADT/STLExtras.h>
#include <llvm/ADT/iterator.h>
#include <llvm/ADT/ilist.h>
#include <iostream>
#include <list>
#include <vector>

template<
    typename WrappedIteratorT,
    typename T = decltype(&*std::declval<WrappedIteratorT>())>
class pointer_iterator
    : public llvm::iterator_adaptor_base<
          pointer_iterator<WrappedIteratorT, T>,
          WrappedIteratorT,
          typename std::iterator_traits<WrappedIteratorT>::iterator_category,
          T> {
    mutable T Ptr;

  public:
    pointer_iterator() = default;

    explicit pointer_iterator(WrappedIteratorT u)
        : pointer_iterator::iterator_adaptor_base(std::move(u)) {}

    // T operator*() const { return &*this->I; }

    T operator*() const {
        Ptr = &*this->I;
        // std::cout << "inner pointer addr: " << &Ptr << '\n';
        // std::cout << "inner point to: " << Ptr << '\n';
        return Ptr;
    }
};

template<
    typename RangeT,
    typename WrappedIteratorT = decltype(std::begin(std::declval<RangeT>()))>
llvm::iterator_range<pointer_iterator<WrappedIteratorT>>
make_pointer_range(RangeT &&Range) {
    using PointerIteratorT = pointer_iterator<WrappedIteratorT>;
    return make_range(
        PointerIteratorT(std::begin(std::forward<RangeT>(Range))),
        PointerIteratorT(std::end(std::forward<RangeT>(Range))));
}

struct MA : llvm::ilist_node<MA> {
    int i;

    MA(int i) : i(i) {}
};

int main() {
    llvm::ilist<MA> listt;
    listt.push_back(new MA(1));
    listt.push_back(new MA(2));
    listt.push_back(new MA(3));
    listt.push_back(new MA(4));
    listt.push_back(new MA(5));
    listt.push_back(new MA(6));
    auto BeginIter = std::next(listt.begin());
    auto EndIter = std::prev(listt.end());
    auto PtrIter =
        ::make_pointer_range(llvm::make_range(BeginIter, EndIter));
    /* auto i = PtrIter.end(); *i; */
    auto EarlyIncPtrIter = llvm::make_early_inc_range(PtrIter);
    for (auto it = EarlyIncPtrIter.begin(); it != EarlyIncPtrIter.end(); it++) {
        MA *ptr = *it;
        std::cout << "point to: " << ptr << '\n';
        std::cout << "value: " << ptr->i << '\n';

        listt.erase(*ptr);
    }
    for (auto const &ref : listt) { std::cout << ref.i << '\n'; }

    return 0;
}
