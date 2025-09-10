<!--
SPDX-FileCopyrightText: 2021 Shuai Zhang

SPDX-License-Identifier: CC-BY-NC-ND-4.0
-->

# References

## Core Language

- *Accelerated C++*: Recommended for a quick start, but after reading this article you probably no longer need it; also it is a bit dated.
- <https://en.cppreference.com/w/>: Quick reference for the language and the standard library.
- <https://isocpp.org/wiki/faq>: Official FAQ. If you have never read it, skim it once to form an impression.
- <https://abseil.io/tips/>: Tips collected by Google while developing the Abseil library. Most are about C++ itself rather than Abseil specifically.
- *The Design and Evolution of C++*: Historical background written by the creator of C++. Helps deepen understanding; can be read like a story.
- *Imperfect C++*: Discusses various issues with the language and how to work around them.
- *C++ Coding Standards: 101 Rules, Guidelines, and Best Practices*
- *Exceptional C++* & *More Exceptional C++*: Recommended if you plan to use exceptions extensively.

Some books I personally do not recommend at this stage (personal opinion only; open to discussion):

- *The C++ Standard Library*: Fine to use as a dictionary, but with cppreference it may be unnecessary.
- *C++ Primer*: Too thick. If you already have experience with other languages, you don't need that much core-language material; most problems early on are around the ecosystem rather than the core language.
- *Modern C++ Design: Generic Programming and Design Patterns Applied*: Asking beginners to dive into heavy template metaprogramming? Just use generics well—don't torture yourself. If you really need code generation, consider using Python to emit C++ unless templates are especially convenient (e.g., `std::tuple`). Also, `constexpr` keeps getting more powerful.

## Project Engineering

(No references yet.)

## Engineering Practice

- "Linux Multithreaded Server Programming: Using the muduo C++ Network Library" (《Linux 多线程服务端编程：使用 muduo C++ 网络库》)
- *C++ Concurrency in Action: Practical Multithreading*
- [How to Write Performance-Friendly C++ Code (internal document)](https://bytedance.feishu.cn/docs/doccnjhjN5W1PLTjcDgDXDHHZyh)
