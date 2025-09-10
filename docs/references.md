<!--
SPDX-FileCopyrightText: 2021 Shuai Zhang

SPDX-License-Identifier: CC-BY-NC-ND-4.0
-->

# 参考资料

## 语言核心

- *Accelerated C++*：快速入门建议看 ，但是看了这篇文章之后你应该不用看这本书了，而且这本书稍微有点老。
- <https://en.cppreference.com/w/>：库和语言标准速查。
- <https://isocpp.org/wiki/faq>：官方提供的一些常见问题手册，建议没看过的过一遍留个印象。
- <https://abseil.io/tips/>：Google 写 Abseil 库时候想到的一些 Tips，大部分都是和 C++ 本身相关的，并没有锁定到 Abseil 库上。
- *The Design and Evolution of C++*：C++ 之父写的一些历史，对于深度理解 C++ 有帮助，可以当小说看。
- *Imperfect C++*：C++ 语言的一些问题，以及怎么 workaround。
- *C++ Coding Standards: 101 Rules, Guidelines, and Best Practices*
- *Exceptional C++ & More Exceptional C++*：如果想用异常的话建议看看这两本。

还有一些书，我个人不建议在这个时期看（个人观点仅供参考，欢迎讨论）。

- *The C++ Standard Library*：当词典用就行了，但是有了 cppreference 我觉得已经用不上了。
- *C++ Primer*：太厚了，我觉得有其他语言经验的同学，入门学习 C++ 不需要学这么多语言核心相关的东西，更多的问题是周边的问题而不是语言核心。
- *Modern C++ Design: Generic Programming and Design Patterns Applied*：你让初学者去搞模板元编程？好好用泛型就行了，别折腾自己。真的有需要可以考虑一下用 Python 生成一段 C++ 代码，除非这个事情用 C++ 模板做特别舒服（比如说 `std::tuple`）。另外 `constexpr` 也越来越强大了。

## 项目工程化

暂无参考资料

## 工程实践

- 《Linux 多线程服务端编程：使用 muduo C++ 网络库》
- *C++ Concurrency in Action: Practical Multithreading*
- [如何写好对性能友好的C++代码（内网资料）](https://bytedance.feishu.cn/docs/doccnjhjN5W1PLTjcDgDXDHHZyh)
