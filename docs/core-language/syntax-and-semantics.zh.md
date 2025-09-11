<!--
SPDX-FileCopyrightText: 2021 Shuai Zhang

SPDX-License-Identifier: CC-BY-NC-ND-4.0
-->

# 语法语义

## 声明和定义，头文件

由于上古编译器的限制，C/C++ 将函数/类型的声明和定义分离了，这样在整个编译过程中，可以不用带着其实现到处链接，等到链接阶段再把生成出来的代码搞到一起。现阶段（在 Module 落地之前），我们还需要遵循这样的使用原则，对于不局限于当前编译单元的函数/类型，先声明，再使用。

在 C++ 中，约定使用文件名后缀为 `.h` 的文件统一放置所有声明，使用文件名后缀为 `.cc` 或 `.cpp` 的文件统一放置所有实现，使用 `.inc` 文件名后缀放置其他需要 #include 的文本（比如说数据，或者代码片段）。

/// admonition | 注意
如果使用了 template，则这个方法或者类的实现必须都定义在头文件中
///

```cpp
--8<-- ".snippets/syntax-and-semantics/001-header-impl-example.cc:code"
```

## Package 和 Namespace

在 C++ 中使用 Namespace 来实现类似于 Java Package 的能力。但是在 C++ 中没有 package-visible 的可见性级别。

## 只读标记

目前 C++ 中有这样几个修饰符：

1. `const`
1. `constexpr`
1. `consteval`（C++20）
1. `constinit`（C++20）

### `const` 修饰符

`const` 有 2 个含义：

1. 为它所修饰的类型（比如 `int32_t`）创建一个只读版本的类型（比如 `const int32_t`）
1. 声明为常量

对于第 1 个用法，见“程序语言”->“类型系统”->“只读类型”。对于第 2 个用法，见“程序语言”->“类型系统”->“常量”。

### `constexpr`，`consteval` 和 `constinit` 修饰符

由于 `const` 的这两个含义在有些时候是有歧义的，所以将第 2 个用法单独搞了一个关键字 `constexpr`。所以推荐对于第 1 个用法永远使用 `const`，对于第 2 个用法永远使用 `constexpr`。

但是其实 `constexpr`  也有 2 种用法：

1. 声明常量
1. 声明这个函数可以在编译时期求值（这个说法比较不严谨，严格的列表见 <https://en.cppreference.com/w/cpp/language/constexpr>）

第 2 个用法也被拆出来到 `consteval` 修饰符了。

第 1 个方法其实还有一些细分问题，比如之前在如何定义常量的时候提到过一个事情——不要声明 `std::string` 常量（但是可以声明 `char*` 常量）。这里有一些比较深层次的问题——是否能在编译时期创建一个 `std::string`？如果不能的话，我们怎么创建一个 `std::string` 类型的常量？`std::string` 的析构有什么类似的限制么？为了处理这种 non-trivial 类型的常量创建的问题，我们又引入了一个新的修饰符 `constinit`，其详细介绍参考 <https://en.cppreference.com/w/cpp/language/constinit>。

## 虚函数标记

在 Java 中可以使用 `final` 修饰符防止一个类被继承或者一个方法被重写，C++ 也是使用 `final` 修饰符完成这一工作。

在 Java 中使用 `@Override` annotation 来修饰一个被重写的方法，在 C++ 中，我们更进一步也搞了一个 specifier 来搞这个事情。

```cpp
--8<-- ".snippets/syntax-and-semantics/002-virtual-final-override.cc:code"
```

## 接口类型和纯虚函数

C++ 中没有接口类型，我们可以认为一个没有成员变量的抽象类就是一个 interface。由于 C++ 中没有多继承的限制，所以这是成立的。我们使用这样的方法来声明一个纯虚函数：

```cpp
--8<-- ".snippets/syntax-and-semantics/003-pure-virtual-interface.cc:code"
```

/// admonition | 注意
正常情况下按照 Java 的继承姿势创建接口类型或者抽象类型的时候，都应该使用上面提到过的“引用语义”，即：

1. 禁止复制
1. 虚析构函数
1. （最好）允许 move
///

## 传值，传指针，传引用

在 Java 中，可以认为所有 Primitive 类型都是传值的，所有其他类型都是传引用的。但是在 C++ 中，取决于定义类型的时候的语义是什么样的，详情见“程序语言”->“类型系统”->“自定义类型”->“（copy-by）值语义类型/引用语义类型”。

除了约定使用指针的地方，尽量不要使用指针。如果需要表示可能不存在的值，应该使用 `std::optional`，详情见“程序语言”->“类型系统”->“`std::optional`”。

## RAII，所有权和 move 语义

RAII（Resource Acquisition Is Initialization），类似于 Java 中的 try-with-resources 语句，可以通过对象生命周期的控制，来控制资源。比如 Java 中常用这样的模式来控制文件资源的生命周期：

```java
--8<-- ".snippets/syntax-and-semantics/004-raii-java-example.java:code"
```

在 `try` 语句块结束的时候，会自动调用 `BufferedReader` 类型的 `close()` 方法，从而完成底层操作系统文件句柄的关闭。类似的，C++ 可以在析构函数中进行包装，从而完成这样的管理操作：

```cpp
--8<-- ".snippets/syntax-and-semantics/005-raii-cpp-example.cc:code"
```

在 C++ 中，我们经常会选择在栈上分配小对象，栈上对象的生命周期是和其作用域绑定的。有的时候我们需要将管理的资源传递给作用域外的管理者进行管理，此时我们就需要 move 语义，将一个对象内部的内容“转移”给其他对象。

```cpp
--8<-- ".snippets/syntax-and-semantics/006-move-semantics.cc:code"
```

## 运算符重载

C++ 中允许重载运算符，比较常用的用法有这么几种：

1. 重载复制/赋值运算符，这个用法已经在“程序语言”->“类型系统”->“自定义类型”->“（copy-by）值语义类型/引用语义类型”中介绍过了，这里就不再重复了
1. 重载比较运算符，常见于给自定义的 DataType 添加比较自然的比较方式
1. 重载小括号运算符，常见于仿函数（Functor），也就是说把这个类的实例当作一个函数来用
1. 重载大括号运算符，常见于自定义 Container
1. 重载 `<<` 运算符，常见于需要打印内容到日志

/// admonition | 注意
建议不要给运算符重载一些奇怪的语义，用于改变 C++ 的语法约定。尽管这样可以用来构造一些神奇的 DSL，但是不了解背景的其他同学很容易用错。如果不是太复杂，还是用普通函数构造 DSL 好些，可以以一种比较醒目的方式提醒用户这是在干什么。
///

C++ 中重载运算符的方法有 2 种，一种是成员函数，一种是非成员函数：

```cpp
--8<-- ".snippets/syntax-and-semantics/007-operator-overload-declarations.cc:code"
```

如果你需要访问 private 成员变量，那基本上只能采用 `friend` 修饰的成员函数。

### 相等可比较和哈希函数

```cpp
--8<-- ".snippets/syntax-and-semantics/008-equality-comparable.cc:code"
```

通常我们重载相等比较，还希望将这个类型用于 HashMap 等数据结构中，此时我们还需要为这个数据类型提供哈希函数。建议借助 Abseil 的 Hash 库实现之：

```cpp
--8<-- ".snippets/syntax-and-semantics/009-absl-hash-circle.cc:code"
```

### 偏序关系

```cpp
--8<-- ".snippets/syntax-and-semantics/010-strict-weak-order.cc:code"
```

### 全序关系

全序关系需要满足这样的约束：<https://en.cppreference.com/w/cpp/concepts/totally_ordered>

一般是先实现一个类似于 Java 中的 `compareTo()` 方法，然后用之实现下述运算符：

```cpp
--8<-- ".snippets/syntax-and-semantics/011-total-order.cc:code"
```

如果是 C++20 的话，可以用新添加的三相比较运算符 `<=>`：

```cpp
--8<-- ".snippets/syntax-and-semantics/012-spaceship-operator.cc:code"
```

## Lambda 表达式

C++ 的 Lambda 表达式和 Java 类似，但是要求手工列出需要 capture 哪些变量，以及是以何种方式（复制，引用，其他复杂操作）进行 capture。capture all 是一种不要的实践，不建议这么做。

```cpp
--8<-- ".snippets/syntax-and-semantics/013-lambda-capture.cc:code"
```

## C++ 中的函数可以不在类里（特别强调）

所以不要再写这样的代码了：

```java
--8<-- ".snippets/syntax-and-semantics/014-java-util-class.java:code"
```

```cpp
--8<-- ".snippets/syntax-and-semantics/015-cpp-static-class.cc:code"
```

直接写这个方法就行了：

```cpp
--8<-- ".snippets/syntax-and-semantics/016-cpp-free-function.cc:code"
```
