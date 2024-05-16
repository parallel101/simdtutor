# 现代化的 API 设计指南

类型就是最好的注释
Type is all you need

---
---
---

## 结构体传参

---
---
---

```cpp
void foo(string name, int age, int phone, int address);

foo("小彭老师", 24, 12345, 67890);
```
❌ 痛点：参数多，类型相似，容易顺序写错而自己不察觉
❌ 天书：阅读代码时看不见参数名，不清楚每个参数分别代表什么

> 怎么办？

```cpp
struct FooOptions {
    string name;
    int age;
    int phone;
    int address;
};

void foo(FooOptions opts);

foo({.name = "小彭老师", .age = 24, .phone = 12345, .address = 67890});
```

✔️ 优雅，每个参数负责做什么一目了然

---
---
---

也有某些大厂的代码规范推崇注释参数名来增强可读性：

```cpp
foo(/*name=*/"小彭老师", /*age=*/24, /*phone=*/12345, /*address=*/67890);
```

但注释可以骗人：

```cpp
// 这里 age 和 phone 参数写反了！但阅读者如果不看下 foo 的定义，根本发现不了
foo(/*name=*/"小彭老师", /*phone=*/12345, /*age=*/24, /*address=*/67890);
```

而代码不会：

```cpp
// 即使顺序写错，只要名字写对依然可以正常运行
foo({.name = "小彭老师", .phone = 12345, .age = 24, .address = 67890});
```

> 总之，好的 API 设计绝不会给人留下犯错的机会！

---
---
---

再来看一个场景，假设foo内部需要把所有参数转发给另一个函数bar：
```cpp
void bar(int index, string name, int age, int phone, int address);

void foo(string name, int age, int phone, int address) {
    bar(get_hash_index(name), name, age, phone, address);
}
```
❌ 痛点：你需要不断地复制粘贴所有这些参数，非常容易抄错

> 怎么办？

```cpp
struct FooOptions {
    string name;
    int age;
    int phone;
    int address;
};

void bar(int index, FooOptions opts);

void foo(FooOptions opts) {
    // 所有逻辑上相关的参数全合并成一个结构体，方便使用更方便阅读
    bar(get_hash_index(opts.name), opts);
}
```

✔️ 优雅

---
---
---

当老板要求你增加一个参数 sex，加在 age 后面：
```diff
-void foo(string name, int age, int phone, int address);
+void foo(string name, int age, int sex, int phone, int address);
```

你手忙脚乱地打开所有调用了 foo 的文件，发现有大量地方需要修改...

而优雅的 API 设计师小彭老师只需轻轻修改一处：

```cpp
struct FooOptions {
    string name;
    int age;
    int sex = 0; // 令 sex 默认为 0
    int phone;
    int address;
};
```

所有的老代码依然照常调用新的 foo 函数，未指定的 sex 会具有结构体里定义的默认值 0：

```cpp
foo({.name = "小彭老师", .phone = 12345, .age = 24, .address = 67890});
```

---
---
---

## 返回一个结构体

当你需要多个返回值时：不要返回 pair 或 tuple！

一些 STL 容器的 API 设计是反面典型，例如：
```cpp
std::pair<bool, iterator> insert(std::pair<K, V> entry);
```

用的时候每次都要想一下，到底第一个是 bool 还是第二个是 bool 来着？然后看一眼 IDE 提示，才反应过来。
```cpp
auto result = map.insert({"hello", "world"});

cout << "是否成功: " << result.first << '\n';
cout << "插入到位置: " << result.second << '\n';
```

first？second？这算什么鬼？

更好的做法是返回一个定制的结构体：
```cpp
struct insert_result_t {
    bool success;
    iterator position;
};

insert_result_t insert(std::pair<K, V> entry);
```

直接通过名字访问成员，语义清晰明确，我管你是第一个第二个，我只想要表示“是否成功(success)”的那个变量。
```cpp
auto result = map.insert({"hello", "world"});

cout << "是否成功: " << result.success << '\n';
cout << "插入到位置: " << result.position << '\n';
```

最好当然是返回和参数类型都是结构体：
```cpp
struct insert_result_t {
    bool success;
    iterator position;
};

struct map_entry_t {
    K key;
    V value;
};

insert_result_t insert(map_entry_t entry);
```

这里说的都比较激进，你可能暂时不会认同，等你大手大脚犯了几个错以后，你自然会心服口服。
小彭老师以前也和你一样是指针仙人，不喜欢强类型，喜欢 `void *` 满天飞，然后随便改两行就蹦出个 Segmentation Fault，指针一时爽，调试火葬场，然后才开始反思。

STL 中依然在大量用 pair 是因为 map 容器出现的很早，历史原因。
我们自己项目的 API 就不要设计成这熊样了。

> 当然，和某些二级指针返回仙人相比 `cudaError_t cudaMalloc(void **pret);`，返回 pair 已经算先进的了

例如 C++17 中的 `from_chars` 函数，他的返回类型就是一个定制的结构体：

```cpp
struct from_chars_result {
    const char *ptr;
    errc ec;
};

from_chars_result from_chars(const char *first, const char *last, int &value);
```

这说明他们也已经意识到了以前动不动返回 pair 的设计是有问题的，已经在新标准中开始改用更好的设计。

---
---
---

## 类型即注释

你是一个新来的员工，看到下面这个函数：
```cpp
void foo(char *x);
```
这里的 x 有可能是：

1. 0结尾字符串，只读，但是作者忘了加 const
2. 指向单个字符，用于返回单个 char（指针返回仙人）
3. 指向一个字符数组缓冲区，用于返回字符串，但缓冲区大小的确定方式未知

如果作者没写文档，变量名又非常含糊，根本不知道这个 x 参数要怎么用。

> 类型写的好，能起到注释的作用！

```cpp
void foo(string x);
```
这样就一目了然了，很明显，是字符串类型的参数。

```cpp
void foo(string &x);
```
看起来是返回一个字符串，但是通过引用传参的方式来返回的

```cpp
string foo();
```
通过常规方式直接返回一个字符串。

```cpp
void foo(vector<uint8_t> x);
```
是一个 8 位无符号整数组成的数组！

```cpp
void foo(span<uint8_t> x);
```
是一个 8 位无符号整数的数组切片。

```cpp
void foo(string_view x);
```
是一个字符串的切片，可能是作者想要避免拷贝开销。

---
---
---

还可以使用类型别名：
```cpp
using ISBN = string;

BookInfo foo(ISBN isbn);
```
这样用户一看就明白，这个函数是接收一个 ISBN 编号（出版刊物都有一个这种编号），返回关于这本书的详细信息。

尽管函数名 foo 让人摸不着头脑，但仅凭直观的类型标识，我们就能函数功能把猜的七七八八。

---
---
---

## 拒绝指针！

注意，这里 foo 返回了一个指针！
```cpp
BookInfo * foo(ISBN isbn);
```

他代表什么意思呢？

1. 指向一个内存中已经存在的书目项，由 foo 负责管理这片内存
2. 返回一个 new 出来的 BookInfo 结构体，由用户负责 delete 释放内存
3. 是否还有可能返回 NULL 表示找不到的情况？
4. 甚至有可能返回的是一个 BookInfo 数组？指针指向数组的首个元素，数组长度的判定方式未知...

太多歧义了！

```cpp
BookInfo & foo(ISBN isbn);
```
这就很清楚，foo 会负责管理 BookInfo 对象的生命周期，用户获得的只是一个临时的引用，并不持有所有权。

引用的特点：

1. 一定不会是 NULL（排除可能返回 NULL 的疑虑）
2. 无法 delete 一个引用（排除可能需要用户负责释放内存的疑虑）
3. 不会用于表示数组（排除可能返回数组首元素指针的疑虑）

改用引用返回值，一下子思路就清晰了很多。没有那么多怀疑和猜测了，用途单一，用法明确，引用真是绝绝子。

```cpp
std::unique_ptr<BookInfo> foo(ISBN isbn);
```
这就很清楚，foo 创建了一个新的 BookInfo，并把生命周期的所有权移交给了用户。

unique_ptr 的特点：

1. 独占所有权，不会与其他线程共享（排除可能多线程竞争的疑虑）
2. 生命周期已经移交给用户，unique_ptr 变量离开用户的作用域后会自动释放，无需手动 delete
3. 不会用于表示数组（如果要表示数组，会用 `unique_ptr<BookInfo[]>` 或者 `vector<BookInfo>`）

但是 unique_ptr 有一个致命的焦虑点：他可以为 NULL！
所以当你看到一个函数返回 unique_ptr 或 shared_ptr，尽管减少了很多的疑虑，但“可能为NULL”的担忧仍然存在！
要么 foo 的作者在注释或文档里写明，“foo 不会返回 NULL”或者“foo 找不到时会返回 NULL”，打消你的疑虑。
但我们的诉求是通过类型，一眼就能看出函数所有的可能性，而不要去依赖可能会骗人的注释。

微软提出了 [gsl](https://github.com/microsoft/GSL) 库，试图解决指针类语义含糊不清的问题：
```cpp
gsl::not_null<std::unique_ptr<BookInfo>> foo(ISBN isbn);
```
他规定，所有套了一层 `gsl::not_null` 的原始指针或智能指针，都必然不会为 NULL。
在 not_null 类的构造函数中，会有相应的断言检查传入的指针是否为空，如果为空会报错退出。

但是，有没有一种可能，你如果要转移所有权的话，直接返回 BookInfo 本身不就行了？
除非 BookInfo 特别大，大到移动返回的开销都不得了。
直接返回类型本身，就是一定不可能为空的，且也能说明移交了对象所有权给用户。
```cpp
BookInfo foo(ISBN isbn);
```

GSL 里大量都是这种可有可无的玩意，比如 C++20 已经有了 std::span 和 std::byte，但是 GSL 还给你弄了个 gsl::span 和 gsl::byte，主要是为了兼容低版本编译器，如果你在新项目里能直接用上 C++20 标准的话，个人不是很推荐再去用了。

再比如 gsl::czstring 是 const char * 的类型别名，明确表示“0结尾字符串”，为的是和“指针返回仙人”区分开来，有必要吗？有没有一种可能，我们现在 const char * 基本上就“0结尾字符串”一种用法，而且我们大多也都是用 string 就可以了，const char * 又不安全，又语义模棱两可，何必再去为了用它专门引入个库，整个类型别名呢？
```cpp
using czstring = const char *;

void foo(czstring s) {          // 发明 GSL 的人真是个天才！
    if (s == "小彭老师")        // 错误！
    if (strcmp(s, "小彭老师"))  // 错误！
    if (!strcmp(s, "小彭老师")) // 终于正确
    // 然而我完全可以直接用 string，== 的运算符重载能直接比较字符串内容
    // 还能随时随地 substr 切片，find 查找，size 常数复杂度查大小
}
```

使用各式各样功能明确的类型和容器，比如 string，vector，或引用。
而不是功能太多的指针，让用户学习你的 API 时产生误解，留下 BUG 隐患。
如果需要指针，也可以通过 const 限定，来告诉用户这个指针是只读的还是可写的。
总之，代码不会撒谎，代码层面能禁止的，能尽量限制用法的，就不要用注释和文档去协商解决。

---
---
---

## 空值语义

---
---
---

有的函数，比如刚才的 foo，会需要表示“可能找不到该书本”的情况。
粗糙的 API 设计者会返回一个指针，然后在文档里说“这个函数可能会返回 NULL！”
```cpp
BookInfo *foo(ISBN isbn);
```
如果是这样的函数签名，是不是你很容易忘记 foo 有可能返回 NULL 表示“找不到书本”？

比如 `malloc` 函数在分配失败时，就会返回 NULL 并设置 errno 为 ENOMEM。
在 `man malloc` 文档中写的清清楚楚，但是谁会记得这个设定？
malloc 完随手就直接访问了（空指针解引用属未定义行为）。

在现代 C++17 中引入了 optional，他是个模板类型。
形如 `optional<T>` 的类型有两种可能的状态：

1. 为空（nullopt）
2. 有值（T）

如果一个函数可能成功返回 T，也可能失败，那就可以让他返回 `optional<T>`，用 nullopt 来表示失败。
```cpp
std::optional<BookInfo> foo(ISBN isbn) {
    if (找到了) {
        return BookInfo(...);
    } else {
        return std::nullopt;
    }
}
```
nullopt 和指针的 nullptr 类似，但 optional 的用途更加单一，更具说明性。
如果你返回个指针人家不一定知道你的意思是可能返回 nullptr，可能还以为你是为了返回个 new 出来的数组，语义不明确。

调用的地方这样写：
```cpp
auto book = foo(isbn);
if (book.has_value()) {  // book.has_vlaue() 为 true，则表示有值
    BookInfo realBook = book.value();
    print("找到了:", realBook);
} else {
    print("找不到这本书");
}
```

optional 类型可以在 if 条件中自动转换为 bool，判断是否有值，等价于 `.has_value()`：
```cpp
auto book = foo(isbn);
if (book) {  // (bool)book 为 true，则表示有值
    BookInfo realBook = book.value();
    print("找到了:", realBook);
} else {
    print("找不到这本书");
}
```

可以通过 * 运算符读取其中的值，等价于 `.value()`）：
```cpp
auto book = foo(isbn);
if (book) {
    BookInfo realBook = *book;
    print("找到了:", realBook);
} else {
    print("找不到这本书");
}
```

运用 C++17 的就地 if 语法：
```cpp
if (auto book = foo(isbn); book.has_value()) {
    BookInfo realBook = *book;
    print("找到了:", realBook);
} else {
    print("找不到这本书");
}
```

由于 auto 出来的 optional 变量可以转换为 bool，分号后面的条件可以省略：
```cpp
if (auto book = foo(isbn)) {
    print("找到了:", *book);
} else {
    print("找不到这本书");
}
```

optional 也支持 `->` 运算符访问成员：
```cpp
if (auto book = foo(isbn)) {
    print("找到了:", book->name);
    book->readOnline();
}
```

optional 的 `.value()`，如果没有值，会抛出 `std::bad_optional_access` 异常。
用这个方法可以便捷地把“找不到书本”转换为异常抛出给上游调用者，而不用成堆的 if 判断和返回。
```cpp
BookInfo book = foo(isbn).value();
```

也可以通过 `.value_or(默认值)` 指定“找不到书本”时的默认值：
```cpp
BookInfo defaultBook;
BookInfo book = foo(isbn).value_or(defaultBook);
```

---
---
---

你接手了一个字符串转整数（可能转换失败）的函数 API：
```cpp
// 文档：如果字符串解析失败，会返回 -1 并设置 errno 为 EINVAL！记得检查！若你忘记检查后果自负！
// 当指定 n 为 0 时，str 为 C 语言经典款 0 结尾字符串。
// 当指定 n 不为 0 时，str 的长度固定为 n，用于照顾参数可能不为 0 结尾字符串的情况。
int parseInt(const char *str, size_t n);
```
那么我如果检测到 -1，鬼知道是字符串里的数字就是 -1，还是因为出错才返回 -1？还要我去检查 errno，万一上一个函数出错留下的 EINVAL 呢？万一我忘记检查呢？

运用本期课程所学知识优化：
```cpp
std::optional<int> parseInt(std::string_view str);
```
是不是功能，返回值，可能存在的错误情况，一目了然了？根本不需要什么难懂的注释，文档。

如果调用者想假定字符串解析不会出错：
```cpp
parseInt("233").value();
```

如果调用者想当出错时默认返回 0：
```cpp
parseInt("233").value_or(0);
```

parseInt 内部实现可能如下：
```cpp
std::optional<int> parseInt(std::string_view sv) {
    int value;
    auto result = std::from_chars(str.data(), str.data() + str.size(), std::ref(value));
    if (result.ec == std::errc())
        return value;
    else
        return std::nullopt;
}
```

---
---
---

调用者的参数不论是 string 还是 C 语言风格的 const char *，都能隐式转换为通用的 string_view。
```cpp
parseInt("-1");

string s;
cin >> s;
parseInt(s);

char performance_geek[2] = {'-', '1'};
parseInt(std::string_view{performance_geek, 2});
```

笑点解析：上面的代码有一处错误，你能发觉吗？

---
---
---

```cpp
cin >> s;
```

`cin >>` 可能会失败！没 想 到 吧

对于 int 等 POD 类型，如果不检测，会出现未初始化的 int 值，导致未定义行为：
```cpp
int i;
cin >> i;
return i;  // 如果用户没有输入值，那么这里会产生典中典之内存中的随机数烫烫烫烤馄饨！
```

正确的做法是要检测 cin 是否失败：
```cpp
int i;
if (!(cin >> i)) {
    throw std::runtime_error("读入 int 变量失败！");
}
return i;
```

但是谁记得住？所以从一开始就不要设计这种糟糕的 API，特别是 `cin >>` 这种通过引用返回 i，却要人记得判断返回 bool 表示成败，忘记判断还会给你留着未初始化的煞笔设计。如果让我来设计 cin 的话：
```cpp
int i = cin.getInt().value();
```
这样如果用户要读取到值的话，必然要 `.value()`，从而如果 getInt 失败返回的是 nullopt，就必然抛出异常，避免了用户忘记判断错误的可能。

> 在小彭老师自主研发的 co_async 协程库中，重新设计了异步的字符流，就是返回 `std::expected`，把成败与返回值二合一。

---
---
---

```cpp
BookInfo * foo(ISBN isbn);
```
这是个返回智能指针的函数，单从函数声明来看，你能否知道他有没有可能返回空指针？不确定。

```cpp
std::optional<BookInfo *> foo(ISBN isbn);
```
现在是不是很明确了，如果返回的是 nullopt，则表示空，然后 optional 内部的 BookInfo *，大概是不会为 NULL 的？

```cpp
std::optional<gsl::not_null<BookInfo *>> foo(ISBN isbn);
```
这下更明确了，如果返回的是 nullopt，则表示空，然后 optional 内部的 BookInfo * 因为套了一层 gsl::not_null，必定不能为 NULL（否则会被 gsl::not_null 的断言检测到），函数的作者是绝对不会故意返回个 NULL 的。
如果失败，会返回 nullopt，而不是意义不明还容易忘记的空指针。

```cpp
std::optional<gsl::not_null<std::unique_ptr<BookInfo>>> foo(ISBN isbn);
```
这个函数可能返回 nullopt 表示失败，成功则返回一个享有所有权的独占指针，指向单个对象。

```cpp
std::optional<std::reference_wrapper<BookInfo>> foo(ISBN isbn);
```
这个函数可能返回 nullopt 表示失败，成功则返回一个不享有所有权的引用，指向单个对象。

reference_wrapper 是引用的包装，可隐式转换为引用：
```cpp
int i;
std::reference_wrapper<int> ref(i);
int &r = ref; // r 指向 i
```
使引用可以存到各种容器里：
且遇到 auto 不会自动退化（decay）：
```cpp
int i;
std::reference_wrapper<int> ref(i);
auto ref2 = ref;  // ref2 推导为 std::reference_wrapper<int>
int &r = i;
auto r2 = r;  // r2 推导为 int
```
且永远不会为 NULL：
```cpp
std::reference_wrapper<int> ref; // 编译错误：引用必须初始化，reference_wrapper 当然也必须初始化
```
也可以通过 `*` 或 `->` 解引用：
```cpp
BookInfo book;
std::reference_wrapper<int> refBook(book);
refBook->readOnline();
BookInfo deepCopyBook = *refBook;
```

---
---
---

注意 `.value()` 和 `*` 是有区别的，`*` 不会检测是否为空，不会抛出异常，但更高效。
```cpp
o.value(); // 如果 o 里没有值，会抛出异常
*o;  // 如果 o 里没有值，会产生未定义行为！
o->readOnline();  // 如果 o 里没有值，会产生未定义行为！
```

因此一般会在判断了 optional 不为空以后才会去访问 `*` 和 `->`。而 `.value()` 可以直接访问。
```cpp
print(foo().value()); // .value() 可以直接使用，不用判断
if (auto o = foo()) {
    // 判断过确认不为空了，才能访问 *o
    // 在已经判断过不为空的 if 分支中，用 * 比 .value() 更高效
    print(*o);
}
```

---
---
---

接下来介绍 optional 的一些进阶用法。

```cpp
std::optional<BookInfo> o = BookInfo(1, 2, 3);  // 初始化为 BookInfo 值
std::optional<BookInfo> o;  // 不写时默认初始化为空，等价于 o = std::nullopt
o.emplace(1, 2, 3);  // 就地构造，等价于 o = BookInfo(1, 2, 3); 但不需要移动 BookInfo 了
o.reset();  // 就地销毁，等价于 o = std::nullopt;
```

---
---
---

当不为空时将其中的 int 值加 1，否则保持为空不变，怎么写？
```cpp
std::optional<int> o = cin.getInt();
if (o) {
    o = *o + 1;
}
```

运用 C++23 引入的新函数 transform：
```cpp
std::optional<int> o = cin.getInt();
o = o.transform([] (int n) { return n + 1; });
```

---
---
---

当不为空时将其中的 string 值解析为 int，否则保持为空不变。且解析函数可能失败，失败则也要将 optional 置为空，怎么写？
```cpp
std::optional<string> o = cin.readLine();
std::optional<int> o2;
if (o) {
    o2 = parseInt(*o);
}

std::optional<int> parseInt(std::string_view sv) { ... }
```

运用 C++23 引入的新函数 and_then：
```cpp
auto o = cin.readLine().and_then(parseInt);
```

---
---
---

当找不到指定书籍时，返回一本默认书籍作为替代：
```cpp
auto o = findBook(isbn).value_or(getDefaultBook());
```

缺点：由于 value_or 的参数会提前被求值，即使 findBook 成功找到了书籍，也会执行 getDefaultBook 函数，然后将其作为死亡右值丢弃。如果创建默认书籍的过程很慢，那么就非常低效。

为此，C++23 引入了 or_else 函数。
只有 findBook 找不到时才会执行 lambda 中的函数体：
```cpp
auto o = findBook(isbn).or_else([] {
    cout << "findBook 出错了，现在开始创建默认书籍，非常慢\n";
    return getDefaultBook();
});
```

---
---
---

此类函数都可以反复嵌套：
```cpp
int i = cin.readLine()
    .or_else(getDefaultLine)
    .and_then(parseInt)
    .transform([] (auto i) { return i * 2; })
    .value_or(-1);
```
加入函数式神教，函门！

---
---
---

## 枚举类型

---
---
---

你的老板要求一个设定客户性别的函数：
```cpp
void foo(int sex);
```

老板口头和员工约定说，0表示女，1表示男，2表示自定义。

这谁记得住？设想你是一个新来的员工，看到下面的代码：

```cpp
foo(1);
```

你能猜到这个 1 是什么意思吗？

解决方法是使用枚举类型，给每个数值一个唯一的名字：

```cpp
enum Sex {
    Female = 0,
    Male = 1,
    Custom = 2,
};

void foo(Sex sex);
```

再假设你是一个新来的员工，看到：

```cpp
foo(Male);
```

是不是就一目了然啦？

---
---
---

枚举的值也可以不用写，让编译器自动按 0、1、2 的顺序分配值：
```cpp
enum Sex {
    Female,   // 0
    Male,     // 1
    Custom,   // 2
};
```

可以指定从 1 开始计数：
```cpp
enum Sex {
    Female = 1,
    Male,      // 2
    Custom,    // 3
};
```

---
---
---

但枚举类型还是可以骗人，再假设你是新来的，看到：

```cpp
foo(Male, 24);
```

是不是想当然的感觉这个代码没问题？

但当你看到 foo 准确的函数定义时，傻眼了：

```cpp
void foo(int age, Sex sex);
```

相当于注册了一个 1 岁，性别是 24 的伪人。且程序员很容易看不出问题，编译器也不报错。

为此，C++11 引入了**强类型枚举**：

```cpp
enum class Sex {
    Female = 0,
    Male = 1,
    Custom = 2,
};
```

现在，如果你再不小心把 sex 传入 age 的话，编译器会报错！因为强类型枚举不允许与 int 隐式转换。

而且强类型枚举会需要显式写出 `Sex::` 类型前缀，当你有很多枚举类型时不容易混淆：

```cpp
foo(24, Sex::Male);
```

如果你的 Sex 范围很小，只需要 uint8_t 的内存就够，可以用这个语法指定枚举的“后台类型”：

```cpp
enum class Sex : uint8_t {
    Female = 0,
    Male = 1,
    Custom = 2,
};

static_assert(sizeof(Sex) == 1);
```

---
---
---

假如你的所有 age 都是 int 类型的，但是现在，老板突然心血来潮：

说为了“优化存储空间”，想要把所有 age 改成 uint8_t 类型的！

为了预防未来可能需要改变类型的需求，也是为了可读性，我们可以使用类型别名：

```cpp
using Age = int;

void foo(Age age, Sex sex);
```

这样当老板需要改变底层类型时，只需要改动一行：

```cpp
using Age = uint8_t;
```

就能自动让所有代码都使用 uint8_t 作为 age 了。

---
---
---

但是类型别名毕竟只是别名，并没有强制保障：

```cpp
using Age = int;
using Phone = int;

foo(Age age, Phone phone);

void bar() {
    Age age = 42;
    Phone phone = 12345;

    foo(phone, age); // 不小心写反了！而编译器不会提醒你！
}
```

因为 Age 和 Phone 只是类型别名，实际上还是同样的 int 类型...所以编译器甚至不会有任何警告。

有一种很极端的做法是把 Age 和 Phone 也做成枚举，但没有定义任何值：

```cpp
enum class Age : int {};
enum class Phone : int {};
```

这样用到的时候就只能通过强制转换的语法：

```cpp
foo(Age(42), Phone(12345));
```

并且如果写错顺序，尝试把 Phone 传入 Age 类型的参数，编译器会立即报错，阻止你埋下 BUG 隐患。

---
---
---

小彭老师，我用了你的方法以后，不能做加法了怎么办？
```cpp
Age(42) + Age(1) // 编译器错误！
```

这是因为 Age 是强类型枚举，不能隐式转换为 int 后做加法。

可以定义一个运算符重载：
```cpp
enum class Age : int {};

inline Age operator+(Age a, Age b) {
    return Age((int)a + (int)b);
}
```

或者运用模板元编程，直接让加法运算符对于所有枚举类型都默认生效：
```cpp
template <class T> requires std::is_enum_v<T>
T operator+(T a, T b) {
    using U = std::underlying_type_t<T>;
    return T((U)a + (U)b);
}
```

有时这反而是个优点，比如你可以只定义加法运算符，就可以让 Age 不支持乘法，需要手动转换后才能乘，避免无意中犯错的可能。

---
---
---

小彭老师，我用了你推荐的**强类型枚举**，不支持我最爱的或运算 `|` 了怎么办？

```cpp
enum class OpenFlag {
    Create = 1,
    Read = 2,
    Write = 4,
    Truncate = 8,
    Append = 16,
    Binary = 32,
};

inline OpenFlag operator|(OpenFlag a, OpenFlag b) {
    return OpenFlag((int)a | (int)b);
}

inline OpenFlag operator&(OpenFlag a, OpenFlag b) {
    return OpenFlag((int)a & (int)b);
}

inline OpenFlag operator~(OpenFlag a) {
    return OpenFlag(~(int)a);
}
```

---
---
---

## 强类型封装

---
---
---

小彭老师，我很喜欢强类型枚举这一套，但我的参数不是整数类型，而是 double、string 等类型，怎么办？

```cpp
struct Name {
private:
    std::string value;

public:
    explicit operator std::string() const {
        return value;
    }

    explicit Name(std::string value) : value(value) {}
};
```
这里我们写 explicit 就可以阻止隐式类型转换，起到与强类型枚举类似的作用。

或者运用模板元编程：
```cpp
// 此处使用 CRTP 模式是为了让 Typed 每次都实例化出不同的基类，阻止互相转换
template <class CRTP, class T>
struct Typed {
protected:
    T value;

public:
    explicit operator T() const {
        return value;
    }

    explicit Typed(T value) : value(value) {}
};
```

```cpp
struct Name : Typed<Name, T> {};

struct Meter : Typed<Meter, double> {};

struct Kilometer : Typed<Kilometer, double> {
    operator Meter() const {
        // 允许隐式转换为米
        return Meter(value * 1000);
    }
};

Meter m = Kilometer(1);
// m = Meter(1000);
foo(m);
```
