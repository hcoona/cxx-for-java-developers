<!--
SPDX-FileCopyrightText: 2021 Shuai Zhang

SPDX-License-Identifier: CC-BY-NC-ND-4.0
-->

# Quick Start

This section briefly introduces how to bootstrap a C++ project from scratch and build the target artifact. Only the essential steps are shown here; more detailed topics will be covered later.

## Configure IDE/Text Editor

Read the official Microsoft documentation:

1. [C/C++ for Visual Studio Code](https://code.visualstudio.com/docs/languages/cpp)
1. [VS Code Remote Development](https://code.visualstudio.com/docs/remote/remote-overview)

## Create a Bazel Project

/// admonition | TODO
    type: todo
Migrate to Bazel Module
///

The directory layout looks like this:

```text
--8<-- ".snippets/quickstart/001-structure.txt:code"
```

Leave the top-level `WORKSPACE.bazel` and `BUILD.bazel` empty for now. Refer to the [official Bazel documentation](https://bazel.build/) for their purposes.

```cpp
--8<--
.snippets/quickstart/002a-example_lib.h:code

.snippets/quickstart/002b-example_lib.cc:code

.snippets/quickstart/002c-main.cc:code
--8<--
```

`example/BUILD.bazel`:

```python
--8<-- ".snippets/quickstart/003-build-bazel.bazel:code"
```

Build command:

```bash
--8<-- ".snippets/quickstart/004-build-command.sh:code"
```

After a successful build the executable is produced at: `bazel-bin/example/example`.

## Other Common C/C++ Build Systems

Other frequently used build systems in the C/C++ ecosystem:

1. Autotools + Makefile: the more "native" GNU toolchain combo. See [Introduction to GNU Autotools](https://opensource.com/article/19/7/introduction-gnu-autotools).
1. CMake: the de facto standard in the modern C++ world. See [Effective Modern CMake](https://gist.github.com/mbinna/c61dbb39bca0e4fb7d1f73b0d66a4fd1#file-effective_modern_cmake-md).
1. MSBuild (Visual Studio's build system): somewhat similar to Maven; arguably better designed, but rarely used outside Visual Studio.

Common dependency/package management approaches in C/C++:

1. Rely on the OS distribution's package manager (e.g. apt/rpm/...)
1. Manage yourself (e.g. copy source into a `third_party` folder/use git submodules/custom scripts to fetch & build/...)
1. Use dedicated package managers such as vcpkg/conan
1. Build systems with partial integration capabilities (cmake/bazel/...), still requiring user effort

You must treat C++ dependency management seriously; it's more fragile than Java. A common Java issue is compiling against a newer version of a library, then deploying a lower version at runtime, causing a `NoSuchMethodError`. In C++, due to the historical header/source separation model, things can go wrong more easily. Libraries with lower implementation quality are more prone to complex resource management issues and Undefined Behavior. Another recurring issue is the diamond dependency problem. In Java, if the application runs fine you might ignore it. In C++, however, ABI (Application Binary Interface) incompatibilities are frequent and will almost always cause problems.
