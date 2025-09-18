<!--
SPDX-FileCopyrightText: 2021 Shuai Zhang

SPDX-License-Identifier: CC-BY-NC-ND-4.0
-->

# Types

## Fundamental Types

Target environment: x86_64 + Linux + GCC. Mapping between common Java and C++ fundamental types:

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
Lengths of legacy types like `int` / `long` depend on CPU / OS / compiler; do not assume a fixed size. See <https://en.cppreference.com/w/cpp/language/types> for common correspondences and detailed descriptions.
///

Other fundamental types common in C++ but not present in Java:

- `nullptr_t`: the real type of `nullptr` (the introduction of `nullptr` was to solve the problem of whether `0` is an integer or a null pointer).
- `size_t` / `ssize_t` (`ssize_t` is not from the C++ standard but provided by the POSIX standard; avoid using it when writing your own portable programs, but when you see it in system APIs you should understand its meaning): a sufficiently large integer type, large enough to represent the size of any C++ object, usually the machine word size (32 bits on 32-bit machines, 64 bits on 64-bit machines), though the standard allows it to be smaller or larger.
- Unsigned versions of all integers: `unsigned int`, `uint64_t`, etc.

/// admonition | Note
If code does not conform to the C++ standard it may not be portable—e.g. it might fail to compile on ARM or behave unexpectedly. For variable-length fundamental types, never assume a specific size.
///

Unsigned types are called out separately to emphasize importance. Simply remember these principles:

1. Prefer signed types to represent counts / lengths (STL / system APIs are historical exceptions)
1. Use unsigned types when representing raw byte sequences / memory blocks / bit fields / explicitly non-negative semantics (e.g., use `uint32_t` to represent an IPv4 address)
1. Be especially careful when comparing signed and unsigned types
    1. You can convert both to a signed type with a larger representable range and then compare, e.g. convert `int32_t` and `uint32_t` both to `int64_t` and compare
    1. If you can ensure the signed integer is not negative (use `DCHECK_GE(value, 0)` to check and serve as a hint), convert it to an unsigned integer for comparison, since in this case the unsigned version can represent a larger range of values than the signed version
    1. If you cannot ensure the signed integer is non-negative, first check whether it is negative; if it is negative it must be less than the unsigned integer, otherwise fall back to the previous case

/// admonition | Note
Avoid raw C++ arrays: when passed to a function they decay to raw pointers and length information is lost; raw pointers are unsafe.

Prefer `std::span<int>` (or `absl::Span`), or `const std::array<int, kSize>&` (less common since it encodes the size into the type and propagates into signatures). Or just use `std::vector` as a dynamic array (if using Abseil, `absl::FixedArray` may be better).
///

### C++ Value Category Classification (Optional Reading)

These are the often-heard categories: lvalue, rvalue, xvalue, etc. For most Java-to-C++ application development you can get by without them; for lower-level library work you should learn them. See <https://en.cppreference.com/w/cpp/language/value_category>.

### Extended Discussion on Using Signed Integer to Represent Length

A common error example of using unsigned integers:

```cpp
--8<-- ".snippets/types/primitive-types/001-unsigned-loop.cc:code"
```

/// admonition | TODO
    type: todo
Some discussion: <https://stackoverflow.com/questions/51677855/is-using-an-unsigned-rather-than-signed-int-more-likely-to-cause-bugs-why>. More authoritative sources may be added later.
///

[Google Style Guide](https://google.github.io/styleguide/cppguide.html#Integer_Types) mentions:

> Because of historical accident, the C++ standard also uses unsigned integers to represent the size of containers - many members of the standards body believe this to be a mistake, but it is effectively impossible to fix at this point.

The C++20 standard added the `ssize()` function to return container size expressed as a signed integer <https://en.cppreference.com/w/cpp/iterator/size>.

## User-Defined Types

In principle user-defined types in C++ have the same capabilities as built-in fundamental types. Java differs: built-in numeric types are always value + stack; user-defined types are reference + heap.

### `class` vs `struct`

C++ supports using `class` and `struct` to define user-defined types. In principle the capability of user-defined types is as strong as that of built-in types. The only difference between defining a type with `class` and with `struct` is that default member visibility is private (`class`) vs public (`struct`).

/// admonition | Note
There is no package visibility level in C++.
///

`struct` comes from C; in C++ it is just an alternative spelling for `class` differing only in default visibility. A “simple” C-style struct allows raw memory operations like `memcpy` (otherwise copy constructors etc. matter). Dangerous, but sometimes high-performance. Such simple types are called [POD (deprecated since C++20)](https://en.cppreference.com/w/cpp/named_req/PODType) or [Standard Layout](https://en.cppreference.com/w/cpp/named_req/StandardLayoutType). Details omitted (see Microsoft docs link).

### Constructors and Destructors (Updated 2021-07-18)

All variables must be initialized before use unless you guarantee an assignment precedes every read (which is still initialization). Otherwise the value is indeterminate; memory is not guaranteed to be zeroed. Constructors in both Java and C++ ensure members are initialized.

Unlike Java, in C++ there are three ways to initialize member variables (ordered by recommended usage, and can be approximated as their execution order):

1. Default initialization at the member declaration site
1. Constructor initialization list
1. Constructor body

Prefer initializing members at the declaration site. This avoids omissions when adding constructors and keeps default values near declarations. Initialize with constants / literals this way, e.g.:

```cpp
--8<-- ".snippets/types/user-types/001-person-default-init.h:code"
```

If constructor parameters are required, use the initialization list as much as possible (better performance); fall back to the body only when necessary.

```cpp
--8<-- ".snippets/types/user-types/002-person-ctor-initlist.h:code"
```

/// admonition | Note
Pay special attention: keep the initialization list order aligned with member declaration order.
///

Initialization actually follows the order of member declarations regardless of list order. A mismatch invites confusion and subtle bugs. Example:

```cpp
--8<-- ".snippets/types/user-types/003-person-wrong-order.h:code"
```

The destructor plays a role analogous to Java's [`AutoClosable.close()`](https://docs.oracle.com/javase/8/docs/api/java/lang/AutoCloseable.html#close--) but is implicit: leaving scope invokes it (RAII / try-with-resources like behavior).

```cpp
--8<-- ".snippets/types/user-types/004-file-raii.h:code"
```

/// admonition | Note
In C++, constructors can throw exceptions and be caught, but if a destructor throws an exception it directly causes the program to exit. If a constructor might throw, then in the destructor you must be aware some member variables may not have been properly initialized before throwing.
///

In practice many projects avoid exceptions (see [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html#Exceptions)). Without exceptions a `Status` return type simulates throw/catch. Pattern for constructor-like failure handling:

```cpp
--8<-- ".snippets/types/user-types/005-http-client-factory.h:code"
```

The constructor uses `explicit` because it has a single parameter. Without it an unintended implicit conversion would exist. Avoid such conversions to preserve type safety.

```cpp
--8<-- ".snippets/types/user-types/006-implicit-explicit.h:code"
```

/// admonition | Note
Make constructors minimal: only essential initialization. Defer side effects (e.g. background threads) via separate start methods.
///

Example (background thread): starting one inside a constructor is problematic. Outline:

1. The parent class constructor starts a background thread.
1. The background thread calls a virtual function; at this time the child class constructor has not yet finished executing. Thus the overridden virtual function in the child class may access member variables not yet initialized.

/// admonition | Note
Similarly do not join/wait in the destructor; derived parts may already be gone.
///

/// admonition | Note
Avoid calling virtual functions (directly or indirectly) in constructors/destructors.
///

Suppose there is a base class `Base` and a derived class `Child`. Constructing a `Child` object first calls `Base`'s constructor to initialize `Base`'s members, then calls `Child`'s constructor to initialize `Child`'s members; destructing a `Child` object first destructs `Child`'s members, then `Base`'s members. In this regard C++ and Java are the same.

If a `Base` constructor calls a virtual overridden in `Child`, the `Child` portion is uninitialized—undefined behavior risk. Same for destructors. Hence: avoid.

Sometimes the call chain is indirect: ctor -> helper A -> virtual B. Or `Start()` launches a thread whose callback uses virtual functions; `Stop()` joins it; destructor auto-calls `Stop()` — same pitfall.

/// admonition | Note
<!-- markdownlint-disable-next-line MD033 -->
Calling a virtual in a constructor dispatches only to the base-class implementation (as if non-virtual). <del style="color:#666;">Which can be even more confusing.</del>
///

### (Copy-by) Value-Semantics Types / Reference-Semantics Types

Java user-defined types have a major limitation: it is impossible to define a "value type". In Java, `int` is a value type while `Integer` is a reference type. A significant difference between them is as follows:

```java
--8<-- ".snippets/types/user-types/008-java-value-vs-reference.java:code"
```

/// admonition | Note
Java lets you define only reference types (aside from primitives). C++ defaults to value semantics; extra work is required to force reference-only usage.
///

> For value types, it is best to overload `operator==` and `operator!=` and provide a hash function. We will cover this later in the operator overloading section.

Below is an example of creating a "reference type" in C++:

```cpp
--8<-- ".snippets/types/user-types/007-value-vs-reference.h:code"
```

Summary: a reference-semantics type should:

1. Disable copy
1. Virtual destructor
1. (Preferably) allow move

> More content see <https://isocpp.org/wiki/faq/value-vs-ref-semantics>

/// admonition | Note
If move is disallowed—or copy exists but move not yet provided—explicitly delete the move ctor/assignment.

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

You can create a type alias; using it is identical to using the original. C had `typedef`; C++ added `using` (template-friendly). Prefer `using`.

```cpp
--8<-- ".snippets/types/type-alias/001-basic-aliases.cc:code"
```

Aliases are true synonyms—sometimes convenient, sometimes troublesome (e.g. two semantic roles A and B for the same underlying type but assignment between them is still allowed).

```cpp
--8<-- ".snippets/types/type-alias/002-alias-misuse.cc:code"
```

Workaround: [StrongAlias](https://source.chromium.org/chromium/chromium/src/+/main:base/types/strong_alias.h;drc=14bffe4980429ebe1179319e15e049236252f8c1) and proposal [N3741](https://isocpp.org/blog/2013/08/new-paper-n3741-toward-opaque-typedefs-for-c1y-v2-walter-brown).

## Smart Pointers

### `std::unique_ptr` and `std::shared_ptr`

`std::shared_ptr` and `std::unique_ptr` are “smart pointers” (contrast with raw pointers). Example raw pointer usage:

```cpp
--8<-- ".snippets/types/smart-pointers/001-raw-pointer.cc:code"
```

`new` allocates; later you must `delete`. Remembering every `delete` across complex control flow (early returns, exceptions) is error-prone. A smart pointer is a small value-type wrapper owning a raw pointer whose destructor releases the resource (RAII: Resource Acquisition Is Initialization).

`std::unique_ptr` models exclusive ownership. Copy is disabled. Default to it unless you specifically need shared lifetime (it can be promoted to `std::shared_ptr` via `std::move`, never the reverse). Its destructor invokes `delete` on the held pointer.

`std::shared_ptr` adds reference counting: copying increments a control block count; destruction decrements; reaching zero deletes the managed object.

/// admonition | Note
When using `std::shared_ptr`, be careful to avoid cyclic references (e.g. `a->b->c->a`).
///

Common solution: separate ownership (e.g. singly linked list):

```cpp
--8<-- ".snippets/types/smart-pointers/002-linked-list-ownership.cc:code"
```

Smart pointers can take custom deleters; see documentation.

### `std::shared_ptr` and `std::weak_ptr`

A common scenario is you need to access an object: if it has not been destructed you do something with it; if it has been destructed then do nothing. A tricky point is when you determine the object has not been destructed you must ensure it will not suddenly be destructed during your computation; but if you hold its `std::shared_ptr`, then it will never be destructed because you still hold a reference.

Example: a service exposes `healthy()`; a watchdog monitors it without extending its lifetime unintentionally.

```cpp
--8<-- ".snippets/types/smart-pointers/003-watchdog-weak-ptr.cc:code"
```

## Read-Only Types

In Java the `final` keyword can mark a variable as read-only, but this semantics is problematic especially for reference-type objects.

```java
--8<-- ".snippets/types/const-types/001-final-atomiclong.java:code"
```

Meaning: you cannot rebind `a` to a new `AtomicLong`, but you can still mutate the pointed-to object's internal state.

If you need immutability of the underlying value, you would require an `AtomicLongReadOnlyView`. This pattern is common; C++ offers `const` qualification to carve out a read-only interface.

The `const` keyword qualifies the type to its left, unless there is no type on its left, in which case it qualifies the type immediately to its right. For example `const A*` and `A const*` are equivalent.

```cpp
--8<-- ".snippets/types/const-types/002-const-pointer-and-methods.cc:code"
```

## Constants

Common ways to define constants in C++ (in recommended order):

> More content see https://abseil.io/tips/140

```cpp
--8<-- ".snippets/types/constants/001-define-constants.cc:code"
```

/// admonition | Note
Do not define constants of type `std::string`.
///

Reasoning:

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

The C type system is messy; C++ inherits the baggage. Avoid conversions unless necessary.

### The Four C++-Style Cast Operators

C-style casts are overly powerful. C++ splits them into:

- `const_cast`: can only add or remove the `const` qualifier.
- `reinterpret_cast`: used to convert pointer types (actually does nothing, just changes how we interpret the pointed-to content), or between integers and pointers (sometimes we want to use an integer to represent a pointer, e.g. taking the address directly as its hash value).
- `dynamic_cast`: used to convert between pointers, but checks at runtime via RTTI (Runtime Type Information) whether the conversion type is correct; if not, returns `nullptr`.

Prefer the specialized cast operators; fall back to `static_cast` only when necessary.

### Be Careful with const_cast

/// admonition | Note
If the object is truly `const` (or you cannot prove otherwise) removing `const` and mutating it is undefined behavior.
///

### Converting Base Class Pointer/Reference to Derived Class Pointer/Reference

`dynamic_cast` downcasts with runtime checking. Often logic already guarantees correctness; then use it only in Debug, and `static_cast` in Release. Benefits:

1. No unnecessary runtime type checks; better performance
1. Avoid using RTTI information; resulting code can be smaller

Helper pattern:

```cpp
--8<-- ".snippets/types/conversions/001-implicit-down-cast.h:code"
```

### `bit_cast`

Sometimes we need raw bit reinterpretation (e.g. treat 64 bits as `double`). `bit_cast` (C++20) provides this; before that use `memcpy`.

```cpp
--8<-- ".snippets/types/conversions/002-bit-cast.h:code"
```

### Smart Pointer Type Conversions

#### `std::unique_ptr` can convert to `std::shared_ptr`, but not vice versa

Thus prefer returning `std::unique_ptr`.

```cpp
--8<-- ".snippets/types/conversions/003-unique-to-shared.cc:code"
```

#### `*_pointer_cast`

> <https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast>

`std::static_pointer_cast`, `std::dynamic_pointer_cast`, `std::const_pointer_cast`, `std::reinterpret_pointer_cast` mirror the raw casts for `shared_ptr`. A custom `down_pointer_cast` can also be defined.

```cpp
--8<-- ".snippets/types/conversions/004-pointer-cast.cc:code"
```

### Narrowing Conversions

Implicit narrowing (e.g. `double` → `int`) still occurs with only a warning:

```cpp
--8<-- ".snippets/types/conversions/005-narrowing-basic.cc:code"
```

Hence treat warnings seriously and eliminate them.

To prevent implicit narrowing: prefer brace initialization over parentheses:

```cpp
--8<-- ".snippets/types/conversions/006-brace-init-narrowing.cc:code"
```

When a narrowing conversion is intentional:

```cpp
--8<-- ".snippets/types/conversions/007-narrow-cast-helpers.cc:code"
```

## Generics (C++ Templates)

This is a beginner's guide: only basic generics, no template metaprogramming (many patterns are moving toward `constexpr`).

> Tip: writing a simple recursive algorithm (e.g. Fibonacci) in Haskell helps internalize template meta "pattern matching + recursion" mechanics.

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

Details on headers later. For templates definitions must be visible (typically in headers). Workaround: put implementation in an `.inc` and include it from the header.

```cpp
--8<-- ".snippets/types/generic/005-header-body-layout.cc:code"
```

#### Constraining Types Is Complicated

As of 2021-05-14 Concepts are not widely adopted; constraining templates is clumsy via:

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

For methods this separates into parameter vs return variance.

/// admonition | TODO
    type: todo
Probably most Java users haven't needed formal variance terms; skipping detailed treatment for now. References below.
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

Treat `std::string` primarily as a byte container; encoding concerns are largely absent. Its API differs from other containers; be cautious. (Chromium's stl_utils offers helpers like `Contains`.)

/// admonition | Note
If you need to process Unicode characters, absolutely do not use `std::string`.

> You can consider using the `UnicodeString` type from the ICU project.
///

## Enums

Java enums are rich (methods, fields). C/C++ enums are simple integer sets. Prefer scoped enums; classic enums leak names and lack type safety.

```cpp
--8<-- ".snippets/types/enum/001-basic-enum.cc:code"
```

You can also specify the underlying integer type.

```cpp
--8<-- ".snippets/types/enum/002-underlying-type.cc:code"
```

Legacy style sometimes appears for constants; avoid unless necessary.

```cpp
--8<-- ".snippets/types/enum/003-anonymous-enum-constant.cc:code"
```

## `std::variant` and Tagged Union

Java lacks a native union; C/C++ unions let one memory region represent one of several types (mutually exclusive).

```cpp
--8<-- ".snippets/types/union/001-union-basic.cc:code"
```

To track the active member, add an explicit tag enum.

```cpp
--8<-- ".snippets/types/union/002-union-tagged.cc:code"
```

Old unions struggle with non-trivial types (e.g. `std::string`). Prefer `std::variant`:

```cpp
--8<-- ".snippets/types/union/003-variant-example.cc:code"
```

## `std::optional`

Null pointers historically modeled optional values in C/C++/Java—error-prone. See the classic talk:

[Null References: The Billion Dollar Mistake](https://www.infoq.com/presentations/Null-References-The-Billion-Dollar-Mistake-Tony-Hoare/)

Prefer references over pointers when non-null is required. For maybe-absent values use `std::optional` (`absl::optional`) to make emptiness explicit.

```cpp
--8<-- ".snippets/types/optional/001-optional-basic.cc:code"
```

## std::function

Analogous to Java's `java.util.function`, but allows arbitrary arity and treats all types uniformly.

```java
--8<-- ".snippets/types/function/000-java-functional-example.java:code"
```

```cpp
--8<-- ".snippets/types/function/001-std-function-basic.cc:code"
```

Lambdas have unique closure types; `std::function` type-erases them. Copying a `std::function` copies captured state.

/// admonition | Note
Avoid `std::bind`; see <https://abseil.io/tips/108>
///

`std::function` cannot hold move-only callables; lambdas can capture move-only objects. A move-only wrapper requires a custom workaround:

```cpp
--8<-- ".snippets/types/function/002-std-function-move-only.cc:code"
```
