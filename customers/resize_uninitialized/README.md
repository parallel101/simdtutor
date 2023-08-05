# 提问

dstVec每次都要resize，相当于全拷贝了一次了，在做copy就没意义了，reverse也不能直接设置size，有没有办法去掉这个赋值，比如直接new char[]这种方式，不会默认填充值
```
    std::vector<char> srcVec(300 * 1024 * 1024, 'a');

	std::vector<char> dstVec;
	dstVec.resize(srcVec.size());

	std::copy(std::execution::par, srcVec.begin(), srcVec.end(), dstVec.begin());
```

# 解答

这是一个`resize_uninitialize`的经典需求，很可惜标准库没有提供这样的一个API。

## 方案1：通过魔改allocator的construct方法
```cpp
#include <vector>
#include <utility>
#include <cstdio>

template <class T>
struct NoinitAllocator {
    std::allocator<T> m_base;

    using value_type = T;

    NoinitAllocator() = default;

    template <class ...Args>
    decltype(auto) allocate(Args &&...args) { // 分配与释放功能保持不变，我们只需要劫持构造功能的逻辑
        return m_base.allocate(std::forward<Args>(args)...);
    }

    template <class ...Args>
    decltype(auto) deallocate(Args &&...args) { // 全部给我完美转发过去
        return m_base.deallocate(std::forward<Args>(args)...);
    }

    template <class ...Args>
    void construct(T *p, Args &&...args) {
        if constexpr (!(sizeof...(Args) == 0 && std::is_pod_v<T>)) // 如果是无参构造且类型为POD类型，则不0初始化
            ::new((void *)p) T(std::forward<Args>(args)...);       // 这样的话你仍然可以用 resize(n, 0) 来强制0初始化
    }

    template <class U> // 下面这两个函数是为了伺候 MSVC 编译通过
    constexpr NoinitAllocator(NoinitAllocator<U> const &other) noexcept {}

    constexpr bool operator==(NoinitAllocator<T> const &other) const {
        return this == &other;
    }
};

int main() {
    std::vector<char, NoinitAllocator<char>> dstVec;
    dstVec.resize(10);  // 只会分配空间，不会零初始化
    // 让我们来试验一下
    dstVec[0] = 1;
    dstVec.resize(0);
    dstVec.resize(10);  // 因为capacity已经为10，不会重新分配空间，而由于construct的无参数构造被我们劫持，不会0初始化
    std::printf("%d\n", dstVec[0]); // 应该打印出1
    dstVec.resize(0);
    dstVec.resize(10, 0);  // 仍然可以这样写来强制0初始化
    std::printf("%d\n", dstVec[0]); // 应该打印出0
    return 0;
}
```
在这个网站上可以在线实测：https://godbolt.org/z/qb8EGoTz1

## 方案2：修改值类型为一个Noinit的特殊模板类
```cpp
template <class T>
struct Noinit {
  T value;
  Noinit() {}  // 不是 = default，也不写 : value()，这样一来只要 T 是 POD 类型，value 就不会0初始化
  Noinit(T value_) : value(value_) {}  // 强制初始化的版本（T隐式转换为Noinit<T>）
  operator T const &() const { return value; } // Noinit<T>隐式转换为T
  operator T &() { return value; } // Noinit<T>隐式转换为T
};
std::vector<Noinit<char>> dstVec;
dstVec.resize(1024);
dstVec[0] = Noinit<char>(1);
dstVec.resize(0);
dstVec.resize(1024); // 不会0初始化的resize
std::printf("%d\n", dstVec[0]); // 1
dstVec.resize(0);
dstVec.resize(1024, Noinit<char>(0));  // 强制0初始化
std::printf("%d\n", dstVec[0]); // 0
```

## 方案3：循规蹈矩，完全遵守 ISO C++ 想让你做的
```cpp
std::vector<char> dstVec;
dstVec.assign(srcVec.begin(), srcVec.end());  // 一次性调整大小+赋值，没有额外开销
```
热知识：并行地拷贝过去并不比串行的拷贝快，因为memcpy是一个内存瓶颈（membound）的操作，并行只能加速计算的部分，除非你这里的不是std::copy而是std::transform并且有一些昂贵的计算量（cpubound），否则并行的memcpy不会变快，没有任何收益，只会徒增耗电量。
