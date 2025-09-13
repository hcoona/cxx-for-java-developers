<!--
SPDX-FileCopyrightText: 2021 Shuai Zhang

SPDX-License-Identifier: CC-BY-NC-ND-4.0
-->

# 类型

## 基础类型

（面向 x86_64 + Linux + GCC 的常见环境）Java 与 C++ 常见基础类型对照：

| Java 类型 | C++ 新类型                                            | C++ 旧类型             | 备注 |
| :-------- | :---------------------------------------------------- | :--------------------- | :--- |
| `void`    |                                                       | `void`                 |      |
| `boolean` |                                                       | `bool`                 |      |
| `char`    | `char8_t` (C++20), `char16_t`, `char32_t`             | `char`, `wchar`        |      |
| `byte`    | `int8_t`, `uint8_t`, `std::byte` (C++17) / `gsl::byte`| `char`, `unsigned char`| 应该用无符号类型表示 Byte，但是 Java 不支持无符号类型，所以这里对应了有符号类型 |
| `short`   | `int16_t`                                             | `short`                |      |
| `int`     | `int32_t`                                             | `int`                  |      |
| `long`    | `int64_t`                                             | `long`, `long long`    |      |
| `float`   |                                                       | `float`                |      |
| `double`  |                                                       | `double`               |      |

/// admonition | 注意
`int`/`long` 等旧类型长度依赖 CPU / OS / 编译器，不要假设固定长度。常见的对应关系及 C++ 中基础类型的详细介绍见 <https://en.cppreference.com/w/cpp/language/types>
///

其他 C++ 中常见而 Java 没有的**基础类型**：

- `nullptr_t`：`nullptr` 的真正类型（`nullptr` 的引入是为了解决 `0` 到底是一个整数还是一个空指针的问题）。
- `size_t` / `ssize_t`（`ssize_t` 并非 C++ 标准，而是 POSIX 标准提供的，自己写程序尽量不要使用，但是调用系统 API 等时候看到了应该理解其含义）：足够大的整数类型，大到足以表示任意一个 C++ 对象大小，通常就是机器字长（即 32 位机上就是 32 位，64 位机上就是 64 位），但是标准上允许它更小或更大。
- 各整数的无符号版本：`unsigned int`, `uint64_t` 等

/// admonition | 注意
如果你的代码不符合 C++ 标准，那它可能就不是 Portable 的，比如说在 ARM 机器上可能就不能编译，或者跑起来不符合你的预期。所以对于不固定长度的数据类型，不要假设其长度是某一个特定值。
///

无符号类型这里单拿出来说，以示重视。简单来说，我们记住这几个原则：

1. 表示数量 / 长度优先用有符号类型（STL/ 系统 API 为历史特例）
1. 表示原始字节序列 / 内存块 / 位字段 / 明确非负语义时用无符号类型（例如用 `uint32_t` 表示 IPv4 地址）
1. 在有符号类型和无符号类型之间比较时，需要特别小心
    1. 可以将两者都转换为更大表示范围的有符号类型，然后进行比较，例如将 `int32_t` 和 `uint32_t` 都转换为 `int64_t` 然后进行比较
    1. 如果确保有符号整数不为负（使用 `DCHECK_GE(value, 0)` 进行检查，并起到提示作用），将其转换为无符号整数进行比较，因为在这种情况下无符号版本比有符号版本能表示的值的范围大
    1. 如果不能确保有符号整数不为负，先检查其是否为负，如果为负则一定小于无符号整数，否则转为上一种情况

/// admonition | 注意
原则上不要使用 C++ 的裸数组，因为裸数组在传递给函数的时候会退化成裸指针，你拿不到数组的长度。而且裸指针是很不安全的。

使用 `std::span<int>`（或者 `absl::Span`），`const std::array<int, kSize>&`（不常用，因为会把数组长度搞到类型里，进一步搞到了函数签名上，很恶心），来传递数组。或者干脆就使用 `std::vector` 假装是一个数组（如果你用了 abseil 库的话，可能 `absl::FixedArray` 是一个更好的选择）。
///

### C++ 值语意分类（选读）

这里指可能经常听过的左值（lvalue），右值（rvalue），消亡值（xvalue）等概念。个人认为 Java 转 C++ 开发的常规情况下不需要学习也可以正常使用 C++ 语言进行日常开发，但是如果需要写比较基础的类库的话，需要掌握这些知识。或者比较好奇相关问题的话，也可以看一下。这里不展开介绍了，详情见文档 <https://en.cppreference.com/w/cpp/language/value_category>。

### 关于使用有符号整型表示长度的展开讨论

使用无符号整型的一个常见错误示例：

```cpp
--8<-- ".snippets/types/primitive-types/001-unsigned-loop.cc:code"
```

/// admonition | TODO
    type: todo
一些讨论见 <https://stackoverflow.com/questions/51677855/is-using-an-unsigned-rather-than-signed-int-more-likely-to-cause-bugs-why>，我将来会再找一些更权威的资料。
///

[Google 风格指南](https://google.github.io/styleguide/cppguide.html#Integer_Types) 中提到：

> Because of historical accident, the C++ standard also uses unsigned integers to represent the size of containers - many members of the standards body believe this to be a mistake, but it is effectively impossible to fix at this point.

C++20 标准中新增了 `ssize()` 函数用来返回以有符号整型表示的容器大小 <https://en.cppreference.com/w/cpp/iterator/size>。

## 用户自定义类型

原则上，在 C++ 中，用户自定义类型享有和语言内建基础类型一样的能力。Java 与此不同，内建的数值类型永远是传值的，且分配在栈上；用户自定义类型永远是传引用的，且分配在堆上。

### `class` 与 `struct`

C++ 中支持使用 `class` 和 `struct` 来定义自定义类型。原则上自定义类型的能力和内建（builtin）类型的能力一样强。使用 `class` 和 `struct` 定义类型的**唯一区别**就是默认成员可见性是 private（`class`）还是 public（`struct`）。

/// admonition | 注意
C++ 中没有包可见性级别。
///

`struct` 的概念起源于 C 语言，但是在 C++ 中只是 `class` 的另一个写法，语义上除了默认可见性没有别的区别。在 C 语言中的这种“简单”的 `struct` 相对于目前这种 C++ 的 `class` 有一个显著的优势是，我们可以通过 `memcpy` 等操作直接对一块内存区域进行操作（否则需要考虑拷贝构造函数等等）。这样的操作虽然非常危险，但是有的时候却会对性能有很大的提升。我们称类似于 C 语言中这种“简单”的 `struct` 为 [POD 类型（Plain old data-type，deprecated since C++20）](https://en.cppreference.com/w/cpp/named_req/PODType) 或 [Standard Layout Type](https://en.cppreference.com/w/cpp/named_req/StandardLayoutType)，具体概念不在此展开（可参考 <https://docs.microsoft.com/en-us/cpp/cpp/trivial-standard-layout-and-pod-types?view=msvc-160>）。

### 构造函数与析构函数（Updated 2021-07-18）

所有变量都需要被初始化后再使用，除非你能保证在使用之前先给它赋值（实际上也相当于初始化了），否则的话相当于使用了一个行为不确定的变量的值。在这种情况下，变量的内容不保证是（机器代码语义中的）0，可能是任意内容。在 Java 和 C++ 中，使用构造函数来保证一个类的成员变量都被正确初始化了。

与 Java 不同的是，在 C++ 中有 3 种方式初始化成员变量（按照推荐使用的顺序排序，同时也可以近似认为是他们的执行顺序）：

1. 成员声明处的默认初始化
1. 构造函数初始化列表
1. 构造函数函数体

如有可能，我们应当尽可能地在声明成员变量的同时指定初始化值，这样可以避免我们添加构造函数的时候漏掉一些成员变量，而且这个值和声明的地方比较近，也容易被找到。将成员变量初始化为某个常量/字面量的场景，通常使用这种方式，例如：

```cpp
--8<-- ".snippets/types/user-types/001-person-default-init.h:code"
```

如果需要在构造函数参数中传入一些值用于初始化，那么我们只能选择另外 2 种方式。在这种情况下，我们也应当尽可能在构造函数的初始化列表中进行初始化。这样做性能更好一些。最后别无选择的情况下，再使用函数体。

```cpp
--8<-- ".snippets/types/user-types/002-person-ctor-initlist.h:code"
```

/// admonition | 注意
特别需要注意的是，初始化列表的顺序需要和成员变量声明的顺序保持一致。
///

实际上不管怎么写初始化列表，实际上在进行初始化的时候，都是按照成员变量声明的顺序执行的。所以一旦这两个顺序不一致了，就很容易造成误解。在一些特定情况下，很容易出错，例如：

```cpp
--8<-- ".snippets/types/user-types/003-person-wrong-order.h:code"
```

析构函数就是 C++ 中类似于 [`AutoClosable.close()`](https://docs.oracle.com/javase/8/docs/api/java/lang/AutoCloseable.html#close--) 的方法。可以认为 C++ 中的每一个对象实例都是包含在 Java 的 [try-with-resources](https://docs.oracle.com/javase/tutorial/essential/exceptions/tryResourceClose.html) 块中的，只要超出作用域，就会立刻调用析构函数。

```cpp
--8<-- ".snippets/types/user-types/004-file-raii.h:code"
```

/// admonition | 注意
在 C++ 中，构造函数可以抛出异常并被捕获处理，但是**析构函数抛出异常会直接导致程序退出**。如果在构造函数中可能抛出异常，那么在析构函数中一定要注意，可能有些成员变量还没有来得及被正确初始化，就抛异常出去了。
///

通常实践中，建议不使用异常处理（原因见 [Google C++ Style Guide 相关讨论](https://google.github.io/styleguide/cppguide.html#Exceptions)）。在不使用异常机制的情况下，通常采用返回 `Status` 类的方式来模拟抛异常和捕获异常的过程。在这种情况下，建议使用这样的模式来解决构造函数需要处理并返回错误的情况：

```cpp
--8<-- ".snippets/types/user-types/005-http-client-factory.h:code"
```

注意到这里构造函数使用了 `explicit` 关键字，这是因为该构造函数只有 1 个参数，这种情况下记住一定要用 `explicit` 关键字，否则将会发生参数到该类型的隐式类型转换。隐式类型转换总是我们需要避免的，因为这会破坏我们的类型系统。

```cpp
--8<-- ".snippets/types/user-types/006-implicit-explicit.h:code"
```

/// admonition | 注意
建议构造函数尽可能少做事情，只做必要的初始化工作。其他工作，例如启动后台线程，都可以用上面提到的模式挪到构造函数外面来进行。
///

以后台线程为例，如果在构造函数中启动，就经常会出现问题。这里不给代码了（比较繁琐），大概解释一下是怎么出问题的：

1. 父类构造函数中启动了后台线程。
1. 后台线程调用了虚函数，此时子类构造函数还未执行完成。于是被子类重载的虚函数执行时可能访问了尚未初始化的成员变量。

/// admonition | 注意
同样的道理，不要在析构函数中等待后台线程完成。此时可能子类已经析构了，再调用虚方法的话就跟上面的例子一样了。
///

/// admonition | 注意
不要在构造函数和析构函数中调用虚函数
///

假设有父类 `Base` 和子类 `Child`。那么构造 `Child` 对象时会先调用 `Base` 的构造函数去初始化 `Base` 的成员，然后再调用 `Child` 的构造函数去初始化 `Child` 的成员；析构 `Child` 对象时，会先析构 `Child` 的成员，再析构 `Base` 的成员。在这点上 C++ 和 Java 是一样的。

考虑这样一种情况，如果在 `Base` 的构造函数中调用了一个虚方法，而该方法被 `Child` override 了会如何呢？如果这个函数使用到了 `Child` 的成员，那么可能就会出问题，因为 `Child` 的成员还没有被正确初始化。类似的，在析构函数中调用虚方法也有类似的问题。因此请尽量避免在构造函数/析构函数中调用虚方法。

有的时候这种调用不是那么显然的。稍微不显然的例子是构造函数里面调用函数 A，然后函数 A 调用了虚函数 B。更不显然的例子是 `Start()` 里面开了个后台线程，然后 `Stop()` 去等待这个后台线程退出，为了避免最坏情况下没调用 `Stop()`，在析构函数中调用了 `Stop()`。这个后台线程调用了虚函数。

/// admonition | 注意
<!-- markdownlint-disable-next-line MD033 -->
C++ 中在构造函数中调用虚函数的话，保证只调用其在父类中定义的版本，就好像其不是一个虚方法一样。<del style="color:#666;">其实这样更坑（小声 BB）</del>
///

### （copy-by）值语义类型/引用语义类型

Java 中自定义类型有一个比较大的缺憾，即无法定义“值类型”。在 Java 中，`int` 是一个值类型，而 `Integer` 是一个引用类型，他们之间一个显著的区别如下：

```java
--8<-- ".snippets/types/user-types/008-java-value-vs-reference.java:code"
```

/// admonition | 注意
在 Java 中，我们只能自定义“引用类型”，无法自定义“值类型”。但是在 C++ 却与之有很大不同，我们 **by-default 定义的类型是一个“值类型”**，需要做一些额外的事情才能使其只能用于“引用类型”语义。
///

> 对于值类型，最好重载 `operator==` 和 `operator!=`，并提供 Hash 函数，这部分内容我们会在后面运算符重载小节中详细介绍。

下面给出一个在 C++ 中创建“引用类型”的例子：

```cpp
--8<-- ".snippets/types/user-types/007-value-vs-reference.h:code"
```

简单来说，引用类型应该：

1. 禁止复制
1. 虚析构函数
1. （最好）允许 move

> 更多内容见 <https://isocpp.org/wiki/faq/value-vs-ref-semantics>

/// admonition | 注意
如果不允许 move 的话，或者是实现了 copy 但是暂时先不实现 move，应该显式删除 move 构造函数和赋值运算符

详细解释见 <https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#c21-if-you-define-or-delete-any-copy-move-or-destructor-function-define-or-delete-them-all>
///

/// admonition | TODO
    type: todo
未转录完
///

### 在堆上创建对象

回忆 Java，我们几乎所有的值类型都是在栈上创建的，所有的引用类型都是在堆上创建的：

```java
--8<-- ".snippets/types/user-types/009-java-stack-heap.java:code"
```

在 C++ 中我们是这样处理的：

```cpp
--8<-- ".snippets/types/user-types/010-cpp-stack-heap.cc:code"
```

## 类型别名

在 C++ 中可以生成一个类型的别名，使用别名和使用原名是等价的。在 C 语言中就提供了这样的功能，通过使用 `typedef` 关键字实现。在 C++ 中，因为希望支持 template（后续会介绍），所以引入了新的关键字 `using`。建议总是使用 `using` 定义别名。

```cpp
--8<-- ".snippets/types/type-alias/001-basic-aliases.cc:code"
```

C++ 类型别名等同于原类型，这个特性有的时候挺方便的，有的时候则会带来一些困扰。举个例子，我们有的时候为同一个类型创造了两个不同用途的别名 A 和 B，这个时候我们是不希望将 B 类型的对象赋值给 A 类型的对象的。

```cpp
--8<-- ".snippets/types/type-alias/002-alias-misuse.cc:code"
```

Workaround 的方法见 [StrongAlias](https://source.chromium.org/chromium/chromium/src/+/main:base/types/strong_alias.h;drc=14bffe4980429ebe1179319e15e049236252f8c1)（以及给 C++ 的提案 [New paper: N3741, Toward Opaque Typedefs for C++1Y, v2 -- Walter Brown : Standard C++](https://isocpp.org/blog/2013/08/new-paper-n3741-toward-opaque-typedefs-for-c1y-v2-walter-brown)）

## 智能指针

### `std::unique_ptr` 和 `std::shared_ptr`

`std::shared_ptr` 和 `std::unique_ptr` 在 C++ 中被称为智能指针（Smart Pointers），这是相对于裸指针（Raw Pointers）而言的。举个关于裸指针的例子：

```cpp
--8<-- ".snippets/types/smart-pointers/001-raw-pointer.cc:code"
```

我们通过使用 `new` 关键字在堆上创建一个对象，在使用完成后通过 `delete` 关键字将分配给它的内存空间回收。这里有个关键的问题在于我们必须记住要去 `delete` 它，这对于一些比较复杂的控制流是比较难实现的（比如中间有一些 if-return 的模式）。智能指针相当于是一个分配在堆上的值类型对象，在超出作用域时会自动析构，这样我们就有机会在析构的时候去做点事情，这一概念被称为 RAII（Resource Acquisition Is Initialization，后面会讲）。

`std::unique_ptr` 适用于所有权唯一的情况，它虽然是一个值类型但是却禁用了 copy。简单来说凡是你觉得不需要使用 `std::shared_ptr` 或者不确定是否需要使用 `std::shared_ptr` 的情况下都一律使用 `std::unique_ptr`（因为 `std::unique_ptr` 可以通过 `std::move` 转成 `std::shared_ptr`，但是反过来不行）。它基本上就是对上面 `new` 和 `delete` 的简单封装，在 `std::unique_ptr` 析构的时候会自动调用内部裸指针的 `delete` 函数回收其资源。

`std::shared_ptr` 在 `std::unique_ptr` 的基础上增加了引用计数的功能，相当于是给 `std::unique_ptr` 增加了 copy 功能，但是这里 copy 的不是实际数据，而是给引用计数器 +1，每次 `std::shared_ptr` 析构的时候，将计数器 -1，如果计数器为 0 了，说明这个数据没有人需要了，此时再将其内部数据回收掉。

/// admonition | 注意
使用 `std::shared_ptr` 时，需要注意不要出现环状引用（例如 `a->b->c->a`）。
///

一个常见的解法是把所有权单独处理，以单向链表为例：

```cpp
--8<-- ".snippets/types/smart-pointers/002-linked-list-ownership.cc:code"
```

智能指针还可以自行指定 Deleter 函数，具体用法见相关文档。

### `std::shared_ptr` 和 `std::weak_ptr`

比较常见的一种情况是你需要访问一个对象，如果这个对象还没被析构的话，你就用它做一些事情；如果这个对象已经析构了，那么不干也行。这里有一个比较 tricky 的点是，当你判断这个对象还没被析构的时候，你需要保证在你进行计算过程的时候它不会突然被析构掉；但是如果你持有它的 `std::shared_ptr` 的话，那么它永远都不会被析构掉，因为你这里还持有了一个引用。

上面说的还有点抽象，这里举个例子说明一下。假设说我们有一个服务，可以通过 `healthy()` 方法判断当前这个服务是否正常，服务不正常的时候可能是服务已经卡住了或者什么别的问题；有一个 WatchDog 检测这个类是否在正常工作。

```cpp
--8<-- ".snippets/types/smart-pointers/003-watchdog-weak-ptr.cc:code"
```

## 只读类型

在 Java 中可以使用 `final` 关键字来标记一个变量只读，但是这个只读的语义其实有些问题，特别是对于“引用类型”对象而言。

```java
--8<-- ".snippets/types/const-types/001-final-atomiclong.java:code"
```

这里的实际含义是我们不能再 `new` 一个 `AtomicLong` 并让 `a` 指向这个新的 `AtomicLong` 实例，但是我们仍然可以改变 `a` 当前这个 `AtomicLong` 内部的值。

如果我们想要 `AtomicLong` 内部的值也不可被改变，我们这里实际上需要一个 `AtomicLongReadOnlyView` 对象。在实际使用中，这种“只读”的需求相当普遍，C++ 提供了一种机制，通过对函数进行标注来确定哪些函数是“只读”函数，从而可以自动生成任意对象的只读接口。

`const` 关键字是修饰其左边的类型的，除非其左边没有类型了，此时 `const` 才修饰其右边紧挨着的的类型。例如 `const A*` 和 `A const*` 是等价的。

```cpp
--8<-- ".snippets/types/const-types/002-const-pointer-and-methods.cc:code"
```

## 常量

C++ 中常见的定义常量的方法有以下几种（按照建议顺序排列）：

> 更多内容见 https://abseil.io/tips/140

```cpp
--8<-- ".snippets/types/constants/001-define-constants.cc:code"
```

/// admonition | 注意
特别需要注意的是，不要定义 `std::string` 类型的常量。
///

这是因为：

1. `std::string` 的构造函数直到 C++20 都不支持 `constexpr`。
1. 跨越编译单元（`.cc`，`.cpp`文件）的全局变量之间的初始化顺序是不确定的（Undefined Behavior），这导致的一个问题是如果你的另一个全局变量依赖于这个变量进行初始化的话，其结果是不确定的。（见 <https://isocpp.org/wiki/faq/ctors#static-init-order>）

```cpp
--8<-- ".snippets/types/constants/002-string-constants.cc:code"
```

C++17 之前不能在头文件中定义常量及常量的值，可以使用 `ABSL_INTERNAL_INLINE_CONSTEXPR` 这个宏来 hack 一下。

```cpp
--8<-- ".snippets/types/constants/003-absl-inline-constexpr-comment.cc:code"
```

## 类型转换

> <https://www.modernescpp.com/index.php/c-core-guidelines-rules-for-conversions-and-casts>

C 的类型系统比较混乱，C++ 为了兼容 C 也背了不少历史包袱。所以类型转换这里就成了一个大坑，能避免的话就尽量避免吧。

### C++ Style 四大类型转换操作符

简单的来说，原来的 C-Style 类型转换能力太强了，什么都能转。我们从中拆出来了几个小的分类：

- `const_cast`：只能用于添加或者去除 `const` 修饰符。
- `reinterpret_cast`：用来做指针类型的转换（实际上什么都没做，只是改变了我们对于指针指向的内容的理解），或者整数和指针之间的转换（我们有时候想用整数表示一个指针，比如说直接取地址作为其 Hash 值）。
- `dynamic_cast`：用来做指针之间的转换，但是会通过 RTTI（Runtime Type Information）检查运行时转换的类型是否正确，如果不正确的话返回 `nullptr`。

如果我们能使用这些新的类型转换方法，就使用它们。如果实在不行，就是需要 C-Style 类型转换这么强大的能力，那么可以使用 `static_cast`。

### 小心 const_cast

/// admonition | 注意
特别需要注意的是：如果对象本身是 `const` 的，或者说不能保证不是 `const` 的，即便使用 `const_cast` 去掉了 `const` 修饰符，也不能修改其内容。这个行为是 Undefined Behavior。
///

### 父类指针/引用转换成子类指针/引用

`dynamic_cast` 经常用于将父类型指针转换成子类型指针，例如：`Dog* dog = dynamic_cast<Dog*>(animal)`。通常我们并不是很需要进行运行时类型检查，我们程序的逻辑可以保证这个转换是正确的，使用 `dynamic_cast` 只是为了抓住代码中出现 BUG 的情况。这种情况下我建议在程序的 Debug Build 中使用 `dynamic_cast`，而在 Release Build 中使用 `static_cast`。这样有 2 个好处：

1. 无需进行不必要的运行时类型检查，性能更好
1. 避免使用 RTTI 信息，可以使生成的代码更小

如果我们确实是想要将父类指针转成子类指针，我们还可以做得更好一点：

```cpp
--8<-- ".snippets/types/conversions/001-implicit-down-cast.h:code"
```

### `bit_cast`

除了上面说到的场景，有些时候我们还需要进行一些比较底层（bit-level）的转换。比如说直接将一个 `uint64_t` 转换成一个 double，这个转换不进行任何实际意义上的操作，只是重新理解这 64 bit 二进制内容。这个事情有点类似于 `reinterpret_cast`，但是不符合其定义的转换范围（指针之间，或者指针和整数之间）。在 C++20 中新增了 `bit_cast` 用于解决这个问题。但是在此之前，我们还只能自己使用 `memcpy` 解决一下：

```cpp
--8<-- ".snippets/types/conversions/002-bit-cast.h:code"
```

### 智能指针类型转换

#### `std::unique_ptr` 可以转换成 `std::shared_ptr`，但是反之不行

所以我们如果写自己的方法，应当尽可能地返回 `std::unique_ptr`。

```cpp
--8<-- ".snippets/types/conversions/003-unique-to-shared.cc:code"
```

#### `*_pointer_cast`

> <https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast>

`std::static_pointer_cast`, `std::dynamic_pointer_cast`, `std::const_pointer_cast`, `std::reinterpret_pointer_cast` 是 4 个基本类型转换操作符的 shared_ptr 版本。类似的，我们也可以实现 `down_pointer_cast`。

```cpp
--8<-- ".snippets/types/conversions/004-pointer-cast.cc:code"
```

### 类型收窄

C++ 为了兼容 C 语言，背了不少历史包袱，其中之一就是隐式转换。例如从 `double`（通常需要 8 字节表示）到 `int`（通常需要 4 字节表示）的转换是“自动”的，只会产生一个编译器警告：

```cpp
--8<-- ".snippets/types/conversions/005-narrowing-basic.cc:code"
```

这就是为什么我们需要关注且尽可能消除编译器警告。

首先介绍一下怎么防止出现自动类型转换，原则很简单，就是在调用构造函数的时候总是使用大括号而非小括号：

```cpp
--8<-- ".snippets/types/conversions/006-brace-init-narrowing.cc:code"
```

然后就是需要进行类型转换的时候怎么办：

```cpp
--8<-- ".snippets/types/conversions/007-narrow-cast-helpers.cc:code"
```

## 泛型（C++ 模板）

考虑到这是入门指南，这里只介绍泛型，而不介绍模板元编程（而且在未来的趋势里，C++ 各种奇技淫巧的模板元编程也会逐渐被 `constexpr` 取代）。

> 如果想要深入学习使用模板元编程，强烈建议用 Haskell 写两句递归相关的简单算法（比如说斐波那契数列），然后就明白模板元编程的核心玩法（模式匹配 + 递归）了。

### 泛型方法

```java
--8<-- ".snippets/types/generic/001-generic-method.java:code"
```

```cpp
--8<-- ".snippets/types/generic/002-generic-method.cc:code"
```

### 泛型类

```java
--8<-- ".snippets/types/generic/003-generic-class.java:code"
```

```cpp
--8<-- ".snippets/types/generic/004-generic-class.cc:code"
```

### C++ 中的语法限制

#### 都得写在头文件中

头文件详细概念会在后面介绍。简而言之，就是说原来写在 `.cc` 或者 `.cpp` 文件里的内容现在都得写在 `.h` 里面。如果还是想要分开写 2 个文件的话，作为一个 workaround，也可以把原来写在 `.cc` 文件中的内容写在一个 `.inc` 文件中，然后在 `.h` 文件中 `#include` 这个 `.inc` 文件（实质上还是都写在了 `.h` 文件中）。

```cpp
--8<-- ".snippets/types/generic/005-header-body-layout.cc:code"
```

#### 类型约束的方法比较复杂

C++ 由于目前（2021-05-14）还没有普及 [Concept 机制](https://en.cppreference.com/w/cpp/language/constraints)，对泛型类型进行约束是比较困难且不直接的。目前主要有 2 种机制完成这一工作：

1. 使用 `static_assert` 进行编译时期检查。
2. 使用 `std::enable_if` 在模板展开过程中禁用不符合条件的分支。

举个例子：

```java
public <T extends Number> List<T> fromArrayToList(T[] a) {
    ...
}
```

```cpp
--8<-- ".snippets/types/generic/006-static-assert.cc:code"
```

```cpp
--8<-- ".snippets/types/generic/007-enable-if.cc:code"
```

### 协变和逆变

我们通过继承关系构造一个类型之间的偏序关系：如果类型 A 继承自 Base，则 A ≤ Base。

| 术语 | 含义             |
| ---- | ---------------- |
| 协变 | 保持了 ≤ 序关系  |
| 逆变 | 逆转了 ≤ 序关系  |
| 不变 | 上述两者均不适用 |

具体使用而言，（对方法而言）又分为参数/返回值的协变/逆变。

/// admonition | TODO
    type: todo
考虑了一下，估计就算用 Java 的同学也大部分不知道逆变协变这回事，我先跳过这个 Topic，之后有时间再继续补完。之前查的一些资料还留在这里。
///

模板，协变和逆变

- <https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Covariant_Return_Types>
- <https://quuxplusone.github.io/blog/2019/01/20/covariance-and-contravariance/>
- <https://zhuanlan.zhihu.com/p/228384772>
- <https://cpptruths.blogspot.com/2015/11/covariance-and-contravariance-in-c.html>

| Type | Covariant | Contravariant |
| ---- | ---------:| ------------- |
| STL containers | No | No |
| `std::initializer_list<T *>` | No | No |
| `std::future<T>` | No | No |
| `boost::optional<T>` | No (see note below) | No |
| `std::optional<T>` | No (see note below) | No |
| `std::shared_ptr<T>` | Yes | No |
| `std::unique_ptr<T>` | Yes | No |
| `std::pair<T *, U *>` | Yes | No |
| `std::tuple<T *, U *>` | Yes | No |
| `std::atomic<T *>` | Yes | No |
| `std::function<R *(T *)>` | Yes (in return) | Yes (in arguments) |

Note: `boost::optional<T>` and `std::optional<T>` are not covariant in general because they preserve the value semantics of `T`; some contextual usages may appear covariant but should not be relied upon.

## 字符串

C++ 中的 `std::string` 只能当成 bytes array 来用，基本上不考虑 encoding 相关的问题，当成字符串用的时候一定要慎重。他的很多方法也跟其他容器不太一致，用的时候要多加小心。（可以使用 chromium 中的 stl_utils 解决一些问题，比如 Contains 方法）

/// admonition | 注意
如果需要处理 unicode 字符，一定不要使用 `std::string`。

> 可以考虑使用 icu 项目中的 `UnicodeString` 类型。
///

## 枚举

Java 的枚举类型比较复杂，里面还可以包含方法和成员变量。C/C++ 中的枚举比较纯粹，就是一些特定整数值的集合。C++ 兼容 C-style 声明的枚举类型，但是我们应该尽量避免使用这种形式声明枚举类型，因为这样声明的枚举常量会直接泄漏到当前声明所在的名字空间内，而且也不进行类型检查。我们应该尽量使用新的枚举声明方法，即 scoped-enum。

```cpp
--8<-- ".snippets/types/enum/001-basic-enum.cc:code"
```

更进一步的，我们可以指定 `enum` 的 underlying type，即底层实现使用什么整数类型来存储之：

```cpp
--8<-- ".snippets/types/enum/002-underlying-type.cc:code"
```

由于一些复杂的原因，老的枚举声明有的时候也被用于声明常量，我们知道有这种用法就行了，正常不要这么用。

```cpp
--8<-- ".snippets/types/enum/003-anonymous-enum-constant.cc:code"
```

## `std::variant` 和 tagged union

Java 中没有 Union 类型这个概念。C/C++ 允许程序员以更紧凑的内存结构来表示数据。Union 的主要使用场景是使用同一个类型存储 A 或者 B 类型的数据，两者不会共存。也可以扩展到更多类型。

```cpp
union UnionStorage {
  int32_t n;     // occupies 4 bytes
  uint16_t s[2]; // occupies 4 bytes
  uint8_t c;     // occupies 1 byte
};               // the whole union occupies 4 bytes
```

我们怎么知道实际上 `union` 存储的是什么类型呢，简单的一个方法就是给它再加上一个枚举类型。

```cpp
enum class UnionStorageDataType {
  kUnspecified = 0,
  kInt32 = 1,
  kUint16Array = 2,
  kUint8 = 3,
};

struct UnionStorage {
  UnionStorageDataType data_type;
  union {
    int32_t n;
    uint16_t s[2];
    uint8_t c;
  } data;
};
```

但是这种老式的 `union` 声明有个缺点，就是很难处理 non-trivial 数据类型（比如说 `std::string`）。通常我们使用 `std::variant` 来搞定这个事情：

```cpp
using UnionStorage = absl::variant<
  absl::monostate /* for empty case */, uint64_t,
  std::string, double, std::vector<MyClass>>;

UnionStorage s{3.14};
double* d = absl::get_if<double>(&s);
```

## `std::optional`

C/C++/Java 长期使用 null pointer 来表示 optional value，但是这在工程实践中被证明是容易出错的，感兴趣的同学可以看看下面这个著名的分享。

[Null References: The Billion Dollar Mistake](https://www.infoq.com/presentations/Null-References-The-Billion-Dollar-Mistake-Tony-Hoare/)

所以在 C++ 中，我们引入了 reference 来代替 pointer。其中一个重要的原则就是尽量使用 reference 而非 pointer。尽管 Google C++ Sytle Guide 要求 mutable parameter 使用 pointer 传入，但是我们也基本上假设传入的 pointer 不为空。所以我们该怎么表示一个可能为空的值呢？我个人建议使用 `std::optional`（`absl::optional`），这样可以以一种足够醒目的姿势提示所有人，这里可能为空。

```cpp
absl::optional<Pie> MakePie() {
  bool failed = /* ... */;
  if (failed) {
    return absl::nullopt;
  }

  return Pie();
}
```

## std::function

类似于 Java 的 `java.util.function` 包，但是用起来更方便一些，可以接收任意多参数，也不区分 Primitive Type。

```java
Consumer<String> c = s-> System.out.println(s);
Predicate<String> p = s -> s.isEmpty();
// Function, BiFunction, ...
```

```cpp
std::function<void(absl::string_view)> c = [](absl::string_view s) { LOG(INFO) << s; };
std::function<bool(absl::string_view)> p = [](absl::string_view s) { return s.empty(); };

std::function<Status(int64_t /* id */, absl::string_view /* name */, Gender)> f =
    [](int64_t id, absl::string_view name, Gender gender) {
      // ...
    };
```

lambda 表达式的类型不是 `std::function`，但是 `std::function` 可以绑定一个 lambda 表达式。拷贝一个 `std::function` 的时候会把捕获的内容也拷贝一份。

/// admonition | 注意
避免使用 `std::bind`，详情见 <https://abseil.io/tips/108>
///

`std::function` 不支持绑定 move-only 的类型，但是 lambda 可以捕获 move-only 的类型。目前标准库还没有提供 move-only-function，需要自己 workaround 一下：

```cpp
MoveOnlyInt v(1);
auto lambda = [v = std::move(v)]() { LOG(INFO) << v.value(); };
std::function<void()> f =
    [lambda = std::make_shared<decltype(lambda)>(std::move(lambda))]() {
      lambda();
   };
```
