<!--
SPDX-FileCopyrightText: 2021 Shuai Zhang

SPDX-License-Identifier: CC-BY-NC-ND-4.0
-->

# Background

This document aims to give Java developers the fastest possible on‑ramp to using C++ for everyday project development. It assumes the reader already has a solid understanding of the Java language and real development experience, but—due to various reasons—now needs to work on normal (non-toy) C++ projects. The goal is to introduce the parts of C++ that are frequently used (or frequently misused) in day‑to‑day work so Java developers can become productive quickly, rather than to be a comprehensive C++ tutorial.

Readers who already know some C++ may also benefit, because the language has many subtle details; walking through them systematically can still be valuable. Contributors with deeper C++ knowledge are warmly welcome.

/// admonition | Note
This document is based on the C++11 standard. Some content touches on library features introduced in C++14/17, but those can usually be accessed in C++11 via third‑party libraries (e.g. Abseil). The reason for choosing this baseline is that newer compilers / standards are still not universally deployed; in many real environments you’ll most likely be limited to C++11/14/17.
///

Compared to conventional C++ tutorials:

1. This document compresses discussion of syntax and semantics because C++ and Java are quite similar in these respects; an experienced Java programmer should be able to read C++ code with little friction.
1. It adds comparisons and mapping of the standard library (STL, Standard Template Library) facilities to help Java developers quickly locate familiar “tools”.
1. It emphasizes places where C++ is often misused or easy to misuse, and proposes principled guidelines for handling them.
1. It also introduces parts of the common C++ ecosystem, including but not limited to:
    1. Build systems
    1. Package management
    1. Common third‑party libraries
    1. Common coding idioms
    1. Static code analysis
    1. Dynamic error analysis
    1. Debugging
    1. Profiling

The original internal version (Chinese) was hosted on [ByteDance Feishu Docs](https://bytedance.feishu.cn/docs/doccn2VDrbXZnWjjGoqCeFpVlPc).

/// admonition | Statement
As of (2025-09-10) I’m no longer working in day‑to‑day C++ development. Future updates may occur irregularly based on reader feedback. You’re welcome to leave your ideas and suggestions in the Discussion section below.
///
