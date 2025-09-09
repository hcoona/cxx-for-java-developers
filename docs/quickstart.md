<!-- SPDX-License-Identifier: CC-BY-NC-ND-4.0 -->

# 快速开始

本小节简单介绍如何从 0 开始搭建一个 C++ 开发项目，并编译出目标产物。这里仅做一些简单的介绍，一些比较细节的内容会在后面继续介绍。

## 配置 IDE / 文本编辑器

阅读微软官方文档：

1. [C/C++ for Visual Studio Code](https://code.visualstudio.com/docs/languages/cpp)
1. [VS Code Remote Development](https://code.visualstudio.com/docs/remote/remote-overview)

## 创建 Bazel 项目

/// admonition | TODO
    type: todo
迁移到 Bazel Module
///

项目的目录结构如下所示：

```text
.
├── WORKSPACE.bazel
├── BUILD.bazel
└── example
    ├── BUILD.bazel
    ├── example_lib.h
    ├── example_lib.cc
    └── main.cc
```

顶层的 `WORKSPACE.bazel` 与 `BUILD.bazel` 先留空即可，具体作用参考 [Bazel 官方文档](https://bazel.build/)。

```cpp
// example/example_lib.h
#pragma once

namespace example {

int sum(int a, int b);

}  // namespace example

// example/example_lib.cc
#include "example/example_lib.h"

namespace example {

int sum(int a, int b) { return a + b; }

}  // namespace example

// example/main.cc
#include <iostream>

#include "example/example_lib.h"

int main() {
    int a;
    int b;
    std::cin >> a >> b;
    std::cout << example::sum(a, b) << std::endl;
}
```

`example/BUILD.bazel`：

```python
cc_library(
    name = "example_lib",
    hdrs = ["example_lib.h"],
    srcs = ["example_lib.cc"],
)

cc_binary(
    name = "example",
    srcs = ["main.cc"],
    deps = [":example_lib"],
)
```

编译命令：

```bash
bazel build //example:example
```

构建完成后生成可执行文件：`bazel-bin/example/example`。

## 其他常见 C/C++ 构建系统

C/C++ 世界中比较常见的构建系统还有：

1. Autotools + Makefile：较“原生态”的 GNU 工具链，参见 [Introduction to GNU Autotools](https://opensource.com/article/19/7/introduction-gnu-autotools)。
1. CMake：现代 C++ 世界事实标准，参见 [Effective Modern CMake](https://gist.github.com/mbinna/c61dbb39bca0e4fb7d1f73b0d66a4fd1#file-effective_modern_cmake-md)。
1. MSBuild（Visual Studio 项目的构建系统）：有点类似于 Maven，设计上感觉比 Maven 还要好一点，可惜除了 Visual Studio 应该没别人用。

C/C++ 常见的依赖/包管理方式：

1. 依赖操作系统发行版的包管理器提供（例如 apt/rpm/…）
1. 自己管理（例如 复制代码到 `third_party` 文件夹下/通过 git submodule 提供源代码/自己写脚本拉代码编译/...）
1. 使用 vcpkg / conan 等专门的包管理器
1. 构建系统自己有一定集成能力，但是需要用户做些事情（cmake/bazel/...）

C++ 的依赖管理是需要严肃对待的，比 Java 更容易出问题。Java 依赖管理中比较常见的一类问题就是编译的时候依赖了高版本的库，运行的时候部署了低版本的库，结果用到的某个方法不存在，直接就抛异常了。C++ 由于继承自上古时期的头文件/代码分离的机制，使得整个事情更容易出问题。而且实现质量比较低的第三方库，更容易出现各种复杂的资源管理问题，以及 Undefined Behavior 问题。另一类常见的问题是菱形依赖问题，Java 中这样的问题如果跑起来没事甚至可以不怎么管，但是 C++ ABI（Application Binary Interface）不兼容的情况太多了，基本上都会出问题。
