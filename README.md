<!-- SPDX-License-Identifier: CC-BY-NC-ND-4.0 -->

# C++ for Java Developers: A Starter’s Guide

[中文版 README](./README.zh-Hans.md)

This article was originally written as an internal quick-start guide for Java engineers at ByteDance who needed to begin regular C++ project development. After iterative feedback and additions from colleagues it became fairly comprehensive. The original internal document is no longer externally accessible due to permission changes, so I am publishing my personal backup here for a wider Java audience.

## Purpose

This guide aims to help experienced Java developers become productive in day‑to‑day C++ development as quickly as possible. It assumes solid Java language knowledge and practical engineering experience, and focuses on the C++ features and ecosystem pieces you will most often use (or misuse) in typical projects. It is not meant to be an exhaustive C++ textbook.

Even if you already know some C++, you may still find value here: revisiting the language systematically helps uncover easily forgotten corner cases. Contributions are welcome.

> The baseline is C++11. Some content references library features added in C++14/17. In many environments you can still use them in a C++11 codebase via well‑known third‑party libraries (e.g. Abseil). The choice reflects the reality that newer compilers / standards may not yet be fully deployed everywhere; C++11/14/17 is the pragmatic common subset.

Compared with a traditional C++ tutorial:

1. We compress pure syntax/semantics explanations—Java and C++ share many surface similarities and seasoned Java developers can usually read C++ right away.
1. We add side‑by‑side mapping of commonly used standard library (STL) facilities so you can quickly locate C++ equivalents of familiar Java utilities.
1. We call out pitfalls: areas frequently misused in production C++ and the principles you should follow to avoid them.
1. We introduce selected parts of the surrounding C++ ecosystem, including (but not limited to):
    1. Build systems
    1. Package / dependency management
    1. Common third‑party libraries
    1. Idiomatic coding patterns (idioms)
    1. Static analysis
    1. Dynamic / runtime error analysis tools
    1. Debugging
    1. Profiling

## Building the Documentation Site

### Prerequisites

Install [UV](https://docs.astral.sh/uv/)

```bash
curl -LsSf https://astral.sh/uv/install.sh | sh
```

```powershell
powershell -ExecutionPolicy ByPass -c "irm https://astral.sh/uv/install.ps1 | iex"
```

### Build Commands

```powershell
uv sync --frozen
uv run mkdocs build
```

### Upgrade a Dependency Version

```powershell
uv lock --upgrade <package-name>==<version>
```

## License

Code and documentation use different licenses. See [LICENSE](./LICENSE) for details.

1. Source code: [Apache-2.0](./LICENSE-APACHE-2.0.txt)
2. Documentation: [CC BY-NC-ND 4.0](./LICENSE-CC-BY-NC-ND-4.0.txt)
