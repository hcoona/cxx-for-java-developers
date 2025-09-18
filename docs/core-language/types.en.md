<!--
SPDX-FileCopyrightText: 2021 Shuai Zhang

SPDX-License-Identifier: CC-BY-NC-ND-4.0
-->

# Types

## Fundamental Types

(Targeting a common environment of x86_64 + Linux + GCC) Mapping between common Java and C++ fundamental types:

| Java Type | New C++ Type                                      | Old C++ Type          | Notes |
| :-------- | :------------------------------------------------ | :-------------------- | :---- |
| `void`    |                                                    | `void`                |       |
| `boolean` |                                                    | `bool`                |       |
| `char`    | `char8_t` (C++20), `char16_t`, `char32_t`          | `char`, `wchar`       |       |
| `byte`    | `int8_t`, `uint8_t`, `std::byte` (C++17) / `gsl::byte`| `char`, `unsigned char`| Byte should be represented using an unsigned type, but Java does not support unsigned types, so here it corresponds to a signed type |
| `short`   | `int16_t`                                          | `short`               |       |
| `int`     | `int32_t`                                          | `int`                 |       |
| `long`    | `int64_t`                                          | `long`, `long long`   |       |
| `float`   |                                                    | `float`               |       |
| `double`  |                                                    | `double`              |       |

/// admonition | Note
Lengths of old types like `int` / `long` depend on CPU / OS / compiler; do not assume fixed length. See <https://en.cppreference.com/w/cpp/language/types> for common correspondences and detailed introduction of fundamental types in C++.
///

Other fundamental types common in C++ but not present in Java:

- `nullptr_t`: the real type of `nullptr` (the introduction of `nullptr` was to solve the problem of whether `0` is an integer or a null pointer).
- `size_t` / `ssize_t` (`ssize_t` is not from the C++ standard but provided by the POSIX standard; avoid using it when writing your own portable programs, but when you see it in system APIs you should understand its meaning): a sufficiently large integer type, large enough to represent the size of any C++ object, usually the machine word size (32 bits on 32-bit machines, 64 bits on 64-bit machines), though the standard allows it to be smaller or larger.
- Unsigned versions of all integers: `unsigned int`, `uint64_t`, etc.

/// admonition | Note
If your code does not conform to the C++ standard, it may not be portable — for example, it might not compile on an ARM machine or might not run as expected. Therefore, for variable-length fundamental types, do not assume their length is some specific value.
///

Unsigned types are called out separately to emphasize importance. Simply remember these principles:

1. Prefer signed types to represent counts / lengths (STL / system APIs are historical exceptions)
1. Use unsigned types when representing raw byte sequences / memory blocks / bit fields / explicitly non-negative semantics (e.g., use `uint32_t` to represent an IPv4 address)
1. Be especially careful when comparing signed and unsigned types
    1. You can convert both to a signed type with a larger representable range and then compare, e.g. convert `int32_t` and `uint32_t` both to `int64_t` and compare
    1. If you can ensure the signed integer is not negative (use `DCHECK_GE(value, 0)` to check and serve as a hint), convert it to an unsigned integer for comparison, since in this case the unsigned version can represent a larger range of values than the signed version
    1. If you cannot ensure the signed integer is non-negative, first check whether it is negative; if it is negative it must be less than the unsigned integer, otherwise fall back to the previous case

/// admonition | Note
In principle do not use raw C++ arrays, because raw arrays decay to raw pointers when passed to functions and you cannot obtain the array length. And raw pointers are very unsafe.

Use `std::span<int>` (or `absl::Span`), `const std::array<int, kSize>&` (not common, because it bakes the array length into the type and further into the function signature, which is ugly) to pass arrays. Or simply use `std::vector` pretending it is an array (if you are using the Abseil library, perhaps `absl::FixedArray` is a better choice).
///

### C++ Value Category Classification (Optional Reading)

This refers to frequently heard concepts like lvalue, rvalue, xvalue, etc. Personally, I think for Java developers transitioning to C++, under normal circumstances you can use C++ for daily development without learning these. But if you need to write lower-level libraries you need to master this knowledge. Or if you are curious, you can read about it. Not expanded here; see <https://en.cppreference.com/w/cpp/language/value_category> for details.

### Extended Discussion on Using Signed Integer to Represent Length

A common error example of using unsigned integers:

```cpp
--8<-- ".snippets/types/primitive-types/001-unsigned-loop.cc:code"
```

/// admonition | TODO
    type: todo
Some discussions at <https://stackoverflow.com/questions/51677855/is-using-an-unsigned-rather-than-signed-int-more-likely-to-cause-bugs-why>. I will look for some more authoritative materials in the future.
///

[Google Style Guide](https://google.github.io/styleguide/cppguide.html#Integer_Types) mentions:

> Because of historical accident, the C++ standard also uses unsigned integers to represent the size of containers - many members of the standards body believe this to be a mistake, but it is effectively impossible to fix at this point.

The C++20 standard added the `ssize()` function to return container size expressed as a signed integer <https://en.cppreference.com/w/cpp/iterator/size>.

## User-Defined Types

In principle, in C++, user-defined types enjoy the same capabilities as language built-in fundamental types. Java differs: built-in numeric types are always passed by value and allocated on the stack; user-defined types are always passed by reference and allocated on the heap.

### `class` vs `struct`

C++ supports using `class` and `struct` to define user-defined types. In principle the capability of user-defined types is as strong as that of built-in types. The only difference between defining a type with `class` and with `struct` is that default member visibility is private (`class`) vs public (`struct`).

/// admonition | Note
There is no package visibility level in C++.
///

The concept of `struct` originates from the C language, but in C++ it is just another spelling of `class`; semantically other than default visibility there is no difference. A “simple” C-style `struct` has a notable advantage over current C++ `class`es: we can directly operate on a block of memory with `memcpy` and similar operations (otherwise we must consider copy constructors, etc.). Although such operations are very dangerous, sometimes they bring significant performance improvements. We call “simple” C-style `struct` types [POD types (Plain old data-type, deprecated since C++20)](https://en.cppreference.com/w/cpp/named_req/PODType) or [Standard Layout Type](https://en.cppreference.com/w/cpp/named_req/StandardLayoutType). The concrete concepts are not expanded here (see <https://docs.microsoft.com/en-us/cpp/cpp/trivial-standard-layout-and-pod-types?view=msvc-160>).

### Constructors and Destructors (Updated 2021-07-18)

All variables need to be initialized before use, unless you can guarantee assigning them before use (which effectively also initializes them); otherwise you are using the value of a variable with indeterminate behavior. In such a case, the contents of the variable are not guaranteed to be 0 (in machine code semantics); it could be anything. In both Java and C++, constructors ensure all member variables of a class are properly initialized.

Unlike Java, in C++ there are three ways to initialize member variables (ordered by recommended usage, and can be approximated as their execution order):

1. Default initialization at the member declaration site
1. Constructor initialization list
1. Constructor body

Where possible, we should initialize member variables at the declaration site. This avoids forgetting some members when adding constructors, and the value is close to the declaration making it easier to find. Scenarios of initializing a member variable to some constant / literal usually use this method, for example:

```cpp
--8<-- ".snippets/types/user-types/001-person-default-init.h:code"
```

If you need to pass in some values via constructor parameters for initialization, then you can only choose the other two ways. In that case, we should initialize as much as possible in the constructor's initialization list. This has better performance. Only as a last resort use the constructor body.

```cpp
--8<-- ".snippets/types/user-types/002-person-ctor-initlist.h:code"
```

/// admonition | Note
Pay special attention that the order of the initialization list needs to be consistent with the order of member variable declarations.
///

In fact, regardless of how you write the initialization list, actual initialization executes in the order of member variable declarations. So once these two orders differ, it's easy to cause misunderstanding. In some special cases it's easy to go wrong, for example:

```cpp
--8<-- ".snippets/types/user-types/003-person-wrong-order.h:code"
```

The destructor is analogous to Java's [`AutoClosable.close()`](https://docs.oracle.com/javase/8/docs/api/java/lang/AutoCloseable.html#close--) in C++. You can think every object instance in C++ is contained within a Java [try-with-resources](https://docs.oracle.com/javase/tutorial/essential/exceptions/tryResourceClose.html) block: as soon as it goes out of scope, the destructor is called immediately.

```cpp
--8<-- ".snippets/types/user-types/004-file-raii.h:code"
```

/// admonition | Note
In C++, constructors can throw exceptions and be caught, but if a destructor throws an exception it directly causes the program to exit. If a constructor might throw, then in the destructor you must be aware some member variables may not have been properly initialized before throwing.
///

Usually in practice, it is recommended not to use exception handling (reasons see [Google C++ Style Guide discussion](https://google.github.io/styleguide/cppguide.html#Exceptions)). Without using exceptions, we typically use a `Status` class return value to simulate the process of throwing and catching exceptions. In such cases, it's recommended to use this pattern to handle and return errors that would otherwise be handled in a constructor:

```cpp
--8<-- ".snippets/types/user-types/005-http-client-factory.h:code"
```

Notice the constructor uses the `explicit` keyword because it has only one parameter. In such cases remember to use `explicit`, otherwise implicit conversion from that parameter type to this type will occur. Implicit conversions are always something we need to avoid because they break our type system.

```cpp
--8<-- ".snippets/types/user-types/006-implicit-explicit.h:code"
```

/// admonition | Note
It is recommended that constructors do as little as possible, only necessary initialization work. Other work, such as starting background threads, can be moved outside using the above pattern.
///

Taking a background thread as an example, starting it in a constructor often causes problems. No code here (too cumbersome); roughly how it fails:

1. The parent class constructor starts a background thread.
1. The background thread calls a virtual function; at this time the child class constructor has not yet finished executing. Thus the overridden virtual function in the child class may access member variables not yet initialized.

/// admonition | Note
Similarly, do not wait for a background thread to finish in the destructor. The child class may already be destructed; calling virtual methods then is the same issue.
///

/// admonition | Note
Do not call virtual functions in constructors and destructors.
///

Suppose there is a base class `Base` and a derived class `Child`. Constructing a `Child` object first calls `Base`'s constructor to initialize `Base`'s members, then calls `Child`'s constructor to initialize `Child`'s members; destructing a `Child` object first destructs `Child`'s members, then `Base`'s members. In this regard C++ and Java are the same.

Consider a scenario where the `Base` constructor calls a virtual method overridden by `Child`. If that function uses `Child`'s members, problems may occur because `Child`'s members are not yet properly initialized. Similarly, calling virtual methods in the destructor has similar issues. Therefore avoid calling virtual methods in constructors/destructors.

Sometimes such calls are not obvious. A slightly less obvious example: the constructor calls function A, then function A calls virtual function B. An even less obvious example: `Start()` launches a background thread, then `Stop()` waits for this background thread to exit. To avoid the worst case where `Stop()` is not called, `Stop()` is invoked in the destructor. The background thread calls a virtual function.

/// admonition | Note
<!-- markdownlint-disable-next-line MD033 -->
When calling a virtual function in a constructor in C++, it guarantees only calling the version defined in the base class, as if it were not virtual. <del style="color:#666;">Actually this is even more of a pitfall (quietly muttering)</del>
///

### (Copy-by) Value-Semantics Types / Reference-Semantics Types

Java user-defined types have a major limitation: it is impossible to define a "value type". In Java, `int` is a value type while `Integer` is a reference type. A significant difference between them is as follows:

```java
--8<-- ".snippets/types/user-types/008-java-value-vs-reference.java:code"
```

/// admonition | Note
In Java we can only define reference types and cannot define value types. But in C++ it is very different; the type we define by default is a value type, and we need to do some extra things to make it usable only with reference semantics.
///

> For value types, it is best to overload `operator==` and `operator!=` and provide a hash function. We will cover this later in the operator overloading section.

Below is an example of creating a "reference type" in C++:

```cpp
--8<-- ".snippets/types/user-types/007-value-vs-reference.h:code"
```

In short, a reference type should:

1. Disable copy
1. Virtual destructor
1. (Preferably) allow move

> More content see <https://isocpp.org/wiki/faq/value-vs-ref-semantics>

/// admonition | Note
If move is not allowed, or copy is implemented but move is temporarily not implemented, the move constructor and assignment operator should be explicitly deleted.

Detailed explanation see <https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#c21-if-you-define-or-delete-any-copy-move-or-destructor-function-define-or-delete-them-all>
///

### Creating Objects on the Heap

Recall Java: almost all value types are created on the stack, all reference types are created on the heap:

```java
--8<-- ".snippets/types/user-types/009-java-stack-heap.java:code"
```

In C++ we handle it like this:

```cpp
--8<-- ".snippets/types/user-types/010-cpp-stack-heap.cc:code"
```

## Type Aliases

In C++, you can create an alias for a type; using the alias is equivalent to using the original name. The C language already provided such functionality via the `typedef` keyword. In C++, to support templates (introduced later), a new keyword `using` was added. It is recommended to always use `using` to define aliases.

```cpp
--8<-- ".snippets/types/type-alias/001-basic-aliases.cc:code"
```

C++ type aliases are identical to the original type. This feature is sometimes convenient and sometimes troublesome. For example, sometimes we create two aliases A and B for the same type for different purposes, and we do not wish to assign an object of type B to a variable of type A.

```cpp
--8<-- ".snippets/types/type-alias/002-alias-misuse.cc:code"
```

See the workaround method [StrongAlias](https://source.chromium.org/chromium/chromium/src/+/main:base/types/strong_alias.h;drc=14bffe4980429ebe1179319e15e049236252f8c1) (and the C++ proposal [New paper: N3741, Toward Opaque Typedefs for C++1Y, v2 -- Walter Brown : Standard C++](https://isocpp.org/blog/2013/08/new-paper-n3741-toward-opaque-typedefs-for-c1y-v2-walter-brown)).

## Smart Pointers

### `std::unique_ptr` and `std::shared_ptr`

`std::shared_ptr` and `std::unique_ptr` are called smart pointers in C++, relative to raw pointers. An example about raw pointers:

```cpp
--8<-- ".snippets/types/smart-pointers/001-raw-pointer.cc:code"
```

We create an object on the heap using the `new` keyword and after use reclaim the memory allocated to it with the `delete` keyword. A key problem is we must remember to `delete` it, which is hard to achieve for some complex control flows (such as early returns in if branches). A smart pointer is essentially a value-type object allocated on the heap; when it goes out of scope it is automatically destructed, giving us a chance to do something in the destructor. This concept is called RAII (Resource Acquisition Is Initialization, explained later).

`std::unique_ptr` is suitable where ownership is unique. Although it is a value type, copy is disabled. Simply put, whenever you think you do not need `std::shared_ptr` or are unsure whether you need `std::shared_ptr`, always use `std::unique_ptr` (because a `std::unique_ptr` can be converted into a `std::shared_ptr` via `std::move`, but not vice versa). It basically is a simple wrapper around `new` and `delete`; when a `std::unique_ptr` is destructed it automatically calls `delete` on the internal raw pointer to reclaim its resource.

`std::shared_ptr` adds reference counting functionality on top of `std::unique_ptr`, equivalent to adding copy capability to `std::unique_ptr`, but the copy here is not the actual data; instead it increments the reference counter by 1. Each time a `std::shared_ptr` is destructed it decrements the counter by 1; if the counter reaches 0 it means no one needs this data, and its internal data is reclaimed.

/// admonition | Note
When using `std::shared_ptr`, be careful to avoid cyclic references (e.g. `a->b->c->a`).
///

A common solution is to separate ownership; taking a singly linked list as an example:

```cpp
--8<-- ".snippets/types/smart-pointers/002-linked-list-ownership.cc:code"
```

Smart pointers can also specify a custom deleter function; see relevant documentation for specific usage.

### `std::shared_ptr` and `std::weak_ptr`

A common scenario is you need to access an object: if it has not been destructed you do something with it; if it has been destructed then do nothing. A tricky point is when you determine the object has not been destructed you must ensure it will not suddenly be destructed during your computation; but if you hold its `std::shared_ptr`, then it will never be destructed because you still hold a reference.

Still a bit abstract, so an example: suppose we have a service whose health can be checked by a `healthy()` method; when the service is unhealthy it might be stuck or have some other issue; a WatchDog monitors whether this class is working properly.

```cpp
--8<-- ".snippets/types/smart-pointers/003-watchdog-weak-ptr.cc:code"
```

## Read-Only Types

In Java the `final` keyword can mark a variable as read-only, but this semantics is problematic especially for reference-type objects.

```java
--8<-- ".snippets/types/const-types/001-final-atomiclong.java:code"
```

The actual meaning here is we cannot `new` another `AtomicLong` and let `a` point to this new instance, but we can still change the internal value of the current `AtomicLong` that `a` points to.

If we also want the internal value of `AtomicLong` to be unchangeable, we actually need an `AtomicLongReadOnlyView` object. In actual use this "read-only" need is quite common. C++ provides a mechanism: by annotating functions to indicate which are "read-only" functions, it can automatically generate a read-only interface for any object.

The `const` keyword qualifies the type to its left, unless there is no type on its left, in which case it qualifies the type immediately to its right. For example `const A*` and `A const*` are equivalent.

```cpp
--8<-- ".snippets/types/const-types/002-const-pointer-and-methods.cc:code"
```

## Constants

Common ways to define constants in C++ (ordered by recommendation):

> More content see https://abseil.io/tips/140

```cpp
--8<-- ".snippets/types/constants/001-define-constants.cc:code"
```

/// admonition | Note
Pay special attention: do not define constants of type `std::string`.
///

Reasons:

1. The constructor of `std::string` does not support `constexpr` until C++20.
1. The initialization order of global variables across translation units (`.cc`, `.cpp` files) is undefined behavior, leading to a problem where if another global variable depends on this variable for initialization, the result is indeterminate. (See <https://isocpp.org/wiki/faq/ctors#static-init-order>)

```cpp
--8<-- ".snippets/types/constants/002-string-constants.cc:code"
```

Prior to C++17 you could not define constants and their values in a header file; you could use the macro `ABSL_INTERNAL_INLINE_CONSTEXPR` as a hack.

```cpp
--8<-- ".snippets/types/constants/003-absl-inline-constexpr-comment.cc:code"
```

## Type Conversions

> <https://www.modernescpp.com/index.php/c-core-guidelines-rules-for-conversions-and-casts>

The C type system is rather messy; C++ bears many historical burdens to stay compatible with C. So type conversion becomes a big pit you should avoid whenever possible.

### The Four C++-Style Cast Operators

Simply put, the original C-style cast is too powerful—it can convert anything. We split it into several smaller categories:

- `const_cast`: can only add or remove the `const` qualifier.
- `reinterpret_cast`: used to convert pointer types (actually does nothing, just changes how we interpret the pointed-to content), or between integers and pointers (sometimes we want to use an integer to represent a pointer, e.g. taking the address directly as its hash value).
- `dynamic_cast`: used to convert between pointers, but checks at runtime via RTTI (Runtime Type Information) whether the conversion type is correct; if not, returns `nullptr`.

If we can use these new cast operators, use them. If not—meaning you need the full power of C-style casting—then use `static_cast`.

### Be Careful with const_cast

/// admonition | Note
Pay special attention: if the object itself is `const`, or you cannot guarantee it is not `const`, even if you remove the `const` qualifier with `const_cast`, you still must not modify its contents. Doing so is undefined behavior.
///

### Converting Base Class Pointer/Reference to Derived Class Pointer/Reference

`dynamic_cast` is often used to convert a base type pointer to a derived type pointer, e.g. `Dog* dog = dynamic_cast<Dog*>(animal)`. Usually we do not need runtime type checking; our program logic can guarantee the conversion is correct. Using `dynamic_cast` is just to catch bugs. In that scenario I recommend using `dynamic_cast` in Debug builds and `static_cast` in Release builds. Two benefits:

1. No unnecessary runtime type checks; better performance
1. Avoid using RTTI information; resulting code can be smaller

If we indeed want to convert a base class pointer to a derived class pointer, we can do even better:

```cpp
--8<-- ".snippets/types/conversions/001-implicit-down-cast.h:code"
```

### `bit_cast`

Besides the scenarios above, sometimes we need lower-level (bit-level) conversions. For example, directly converting a `uint64_t` to a `double`. This conversion performs no meaningful operation; it just reinterprets the 64 bits. This is somewhat like `reinterpret_cast`, but outside its defined conversion range (between pointers, or between pointers and integers). C++20 added `bit_cast` to solve this problem. Before that we could only use `memcpy` ourselves:

```cpp
--8<-- ".snippets/types/conversions/002-bit-cast.h:code"
```

### Smart Pointer Type Conversions

#### `std::unique_ptr` can convert to `std::shared_ptr`, but not vice versa

Therefore if we write our own methods, we should return `std::unique_ptr` where possible.

```cpp
--8<-- ".snippets/types/conversions/003-unique-to-shared.cc:code"
```

#### `*_pointer_cast`

> <https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast>

`std::static_pointer_cast`, `std::dynamic_pointer_cast`, `std::const_pointer_cast`, `std::reinterpret_pointer_cast` are the `shared_ptr` versions of the four basic cast operators. Similarly, we can implement `down_pointer_cast`.

```cpp
--8<-- ".snippets/types/conversions/004-pointer-cast.cc:code"
```

### Narrowing Conversions

To stay compatible with C, C++ bears many historical burdens, one of which is implicit conversions. For example, converting from `double` (usually 8 bytes) to `int` (usually 4 bytes) is “automatic” and produces only a compiler warning:

```cpp
--8<-- ".snippets/types/conversions/005-narrowing-basic.cc:code"
```

That is why we need to pay attention to and eliminate compiler warnings as much as possible.

First introduce how to prevent implicit narrowing conversions: the principle is simple—when calling constructors always use braces instead of parentheses:

```cpp
--8<-- ".snippets/types/conversions/006-brace-init-narrowing.cc:code"
```

Then when you do need to convert types:

```cpp
--8<-- ".snippets/types/conversions/007-narrow-cast-helpers.cc:code"
```

## Generics (C++ Templates)

Considering this is a beginner's guide, only generics are introduced here; template metaprogramming is not covered (and in future trends, various template metaprogramming tricks in C++ will gradually be replaced by `constexpr`).

> If you want to dive deeper into template metaprogramming, it is strongly recommended to write a few simple recursive algorithms in Haskell (e.g. Fibonacci). Then you'll understand the core gameplay of template metaprogramming (pattern matching + recursion).

### Generic Methods

```java
--8<-- ".snippets/types/generic/001-generic-method.java:code"
```

```cpp
--8<-- ".snippets/types/generic/002-generic-method.cc:code"
```

### Generic Classes

```java
--8<-- ".snippets/types/generic/003-generic-class.java:code"
```

```cpp
--8<-- ".snippets/types/generic/004-generic-class.cc:code"
```

### Syntax Limitations in C++

#### All Must Be Written in Header Files

The detailed concept of header files will be introduced later. Simply put, content originally written in `.cc` or `.cpp` files must now be written in `.h` files. If you still want to separate into two files, as a workaround you can put what would have been in the `.cc` file into an `.inc` file and then `#include` that `.inc` file inside the `.h` file (essentially still everything is in the `.h`).

```cpp
--8<-- ".snippets/types/generic/005-header-body-layout.cc:code"
```

#### Constraining Types Is Complicated

As of now (2021-05-14) the [Concept mechanism](https://en.cppreference.com/w/cpp/language/constraints) is not yet widely used, making constraining generic types difficult and indirect. Currently there are two main mechanisms:

1. Use `static_assert` for compile-time checks.
2. Use `std::enable_if` to disable non-conforming branches during template instantiation.

Example:

```java
public <T extends Number> List<T> fromArrayToList(T[] a) {
    // ...
}
```

```cpp
--8<-- ".snippets/types/generic/006-static-assert.cc:code"
```

```cpp
--8<-- ".snippets/types/generic/007-enable-if.cc:code"
```

### Covariance and Contravariance

We construct a partial order between types via inheritance: if type A inherits from Base, then A ≤ Base.

| Term | Meaning |
| ---- | ------- |
| Covariance | Preserves the ≤ ordering |
| Contravariance | Reverses the ≤ ordering |
| Invariance | Neither of the above applies |

In concrete usage (for methods) this splits into covariance/contravariance of parameters / return values.

/// admonition | TODO
    type: todo
Gave it some thought; probably most Java users don't really know contravariance/covariance. I'll skip this topic for now and fill it in later. Some previously collected references remain here.
///

Templates, covariance and contravariance

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

## Strings

In C++, `std::string` should only be used as a bytes array, basically ignoring encoding issues; be cautious when using it as a string. Many of its methods are also inconsistent with other containers—use with care. (You can use chromium's stl_utils to solve some problems, e.g. a Contains method.)

/// admonition | Note
If you need to process Unicode characters, absolutely do not use `std::string`.

> You can consider using the `UnicodeString` type from the ICU project.
///

## Enums

Java's enum type is relatively complex; it can contain methods and member variables. C/C++ enums are more pure: just a set of specific integer values. C++ is compatible with C-style enum declarations, but we should avoid using that style because the enum constants leak directly into the containing namespace and there is no type checking. We should prefer the new enum declaration style, i.e. scoped enum.

```cpp
--8<-- ".snippets/types/enum/001-basic-enum.cc:code"
```

Further, we can specify the underlying type of an enum: which integer type to store it as.

```cpp
--8<-- ".snippets/types/enum/002-underlying-type.cc:code"
```

For some complicated reasons, old enum declarations are sometimes used to declare constants. Just know such usage exists; normally don't do this.

```cpp
--8<-- ".snippets/types/enum/003-anonymous-enum-constant.cc:code"
```

## `std::variant` and Tagged Union

Java has no concept of a union type. C/C++ allows programmers to represent data in a more compact memory layout. A union is mainly used to store data of type A or type B (not both simultaneously). Can be extended to more types.

```cpp
--8<-- ".snippets/types/union/001-union-basic.cc:code"
```

How do we know what type the union actually stores? A simple method is to add an enum type.

```cpp
--8<-- ".snippets/types/union/002-union-tagged.cc:code"
```

However, this old-style union declaration has a drawback: it's hard to handle non-trivial data types (e.g. `std::string`). Usually we use `std::variant` to solve this:

```cpp
--8<-- ".snippets/types/union/003-variant-example.cc:code"
```

## `std::optional`

C/C++/Java have long used null pointers to represent optional values, but this is proven error-prone in engineering practice. Interested readers can watch the famous talk below.

[Null References: The Billion Dollar Mistake](https://www.infoq.com/presentations/Null-References-The-Billion-Dollar-Mistake-Tony-Hoare/)

So in C++ we introduced references to replace pointers. One important principle is to use references rather than pointers whenever possible. Although the Google C++ Style Guide requires mutable parameters to be passed via pointers, we basically assume the incoming pointer is non-null. So how do we represent a value that may be empty? I personally recommend using `std::optional` (`absl::optional`), which prominently signals to everyone the value may be empty.

```cpp
--8<-- ".snippets/types/optional/001-optional-basic.cc:code"
```

## std::function

Similar to Java's `java.util.function` package, but more convenient: it can take any number of parameters and does not distinguish primitive types.

```java
--8<-- ".snippets/types/function/000-java-functional-example.java:code"
```

```cpp
--8<-- ".snippets/types/function/001-std-function-basic.cc:code"
```

The type of a lambda expression is not `std::function`, but a `std::function` can bind a lambda expression. Copying a `std::function` copies the captured content as well.

/// admonition | Note
Avoid using `std::bind`; details see <https://abseil.io/tips/108>
///

`std::function` does not support binding move-only types, but a lambda can capture move-only types. Currently the standard library does not yet provide a move-only-function; you need to workaround it yourself:

```cpp
--8<-- ".snippets/types/function/002-std-function-move-only.cc:code"
```
