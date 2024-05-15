# 赞美强类型

```cpp
void foo(string name, int age, int phone, int address);

foo("小彭老师", 24, 12345, 67890);
```
❌ 痛点：参数多，类型相似，容易顺序写错而自己不察觉，编译器又不报错

> 怎么办？

```cpp
struct foo_options {
    string name;
    int age;
    int phone;
    int address;
};

void foo(foo_options opts);

foo({.name = "小彭老师", .age = 24, .phone = 12345, .address = 67890});
```

✔️ 优雅

---
---
---

假设foo内部需要把所有参数转发给另一个函数bar：
```cpp
void bar(int index, string name, int age, int phone, int address);

void foo(string name, int age, int phone, int address) {
    bar(get_hash_index(name), name, age, phone, address);
}
```
❌ 痛点：你需要不断地复制粘贴所有这些参数，非常容易抄错

> 怎么办？

```cpp
struct foo_options {
    string name;
    int age;
    int phone;
    int address;
};

void bar(int index, foo_options opts);

void foo(foo_options opts) {
    // 所有逻辑上相关的参数全合并成一个结构体，方便使用也方便阅读
    bar(get_hash_index(opts.name), opts);
}
```

✔️ 优雅

---
---
---
