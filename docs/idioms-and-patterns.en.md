<!--
SPDX-FileCopyrightText: 2021 Shuai Zhang

SPDX-License-Identifier: CC-BY-NC-ND-4.0
-->

# Common Idioms / Design Patterns

References

- <https://en.wikibooks.org/wiki/C%2B%2B_Programming/Idioms>
- <https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms>

## Background Thread Periodic Activity

> Home-grown, but commonly used

```cpp
--8<-- ".snippets/idioms-and-patterns/001-background-thread-service.cc:code"
```

## Thread-safe Lazy-initialized Singleton

- <https://source.chromium.org/chromium/chromium/src/+/main:base/lazy_instance.h;l=6;drc=7b5337170c1581e4a35399af36253f767674f581>
- <https://www.modernescpp.com/index.php/thread-safe-initialization-of-a-singleton>
- [*Imperfect C++* by Matthew Wilson](http://www.imperfectcplusplus.com/)

/// admonition | Note
Unless we truly need a global object (for example `GlobalBackgroundThreadPool`), we should avoid the Singleton pattern. Prefer passing required objects through constructors or setter functions to achieve better composability and testability.
///

```cpp
--8<-- ".snippets/idioms-and-patterns/002a-singleton-nodestructor.cc:code"
```

Assume for now we ignore what `base::NoDestructor` actually is and pretend it looks like this:

```cpp
--8<-- ".snippets/idioms-and-patterns/002b-singleton-simple.cc:code"
```

1. This only initializes the static `Factory` instance the first time `Factory::GetInstance()` is called.
2. Concurrent multi-threaded calls to `Factory::GetInstance()` are safe; the static instance is initialized exactly once.

/// admonition | TODO
    type: todo

1. Explain why (thread-safe initialization semantics of function-local statics since C++11)
2. Explain what `NoDestructor` does
///

## Implementing Copy-On-Write using `std::shared_ptr`

From the book *Linux Multithreaded Server Programming: Using the muduo C++ Network Library*.

```cpp
--8<-- ".snippets/idioms-and-patterns/003-copy-on-write-queue.cc:code"
```
