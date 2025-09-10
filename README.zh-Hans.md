<!--
SPDX-FileCopyrightText: 2021 Shuai Zhang

SPDX-License-Identifier: Apache-2.0
-->

# 面向 Java 程序员的 C++ 入门指南

[English Readme](./README.md)

这篇文章是我在字节跳动（ByteDance）工作期间写给团队内 Java 程序员的 C++ 入门指南，经过同事们的建议和补充，内容已经比较完善。由于字节跳动内部文档权限调整，[原文档](https://bytedance.feishu.cn/docs/doccn2VDrbXZnWjjGoqCeFpVlPc)已经无法从外部访问，现将我手上的个人备份版本公开，以供更多的 Java 程序员参考。

## 背景介绍

本文旨在为 Java 程序员提供一个最快的入手 C++ 进行常规项目开发的指南。本文假设阅读者具有相当程度对 Java 语言的理解和开发经验，但是出于各种原因需要进行常规的 C++ 项目开发。本文旨在介绍工作中常规 C++ 项目中经常被用到或被误用的内容，从而帮助 Java 程序员快速上手 C++，而非一个面面俱到的 C++ 教程。

已经对 C++ 有一定了解的同学也可能从这篇文章中受益，因为 C++ 的细节问题比较多，这样系统性的过一遍是有好处的。同时也欢迎对 C++ 了解比较多的同学合作共建。

> 本文以 C++11 标准为基础，部分内容涉及到 C++14/17 中引入的新的标准库（STL，Standard Template Library）内容，但是通常这部分内容可以通过第三方库在 C++11 中使用（例如 Abseil）。这么选择是因为高版本的编译器还没有得到很好的普及和应用，大家在这种情况下最有可能用到的基本上也就是 C++11/14/17 了。

相较于常规的 C++ 教程：

1. 本文压缩了关于语法语义方面的知识，因为 C++ 的语法语义和 Java 非常相似，熟练的 Java 程序员应该能无障碍阅读 C++ 代码
1. 本文增加了语言内建提供的标准库的对比，用于帮助 Java 程序员快速找到自己经常使用的工具
1. 本文特别强调了一些 C++ 开发中经常用错或者不容易用对的地方，以及在这些地方应该遵循一个什么样的原则进行处理
1. 本文还介绍了一些常用 C++ 周边生态，包括但不限于：
   1. 构建系统
   1. 包管理
   1. 常见第三方类库
   1. 常见编码 Idiom
   1. 静态代码分析
   1. 动态错误分析
   1. 代码调试
   1. Profiling

## 构建文档站点

### 准备工作

安装 [UV](https://docs.astral.sh/uv/)

```bash
curl -LsSf https://astral.sh/uv/install.sh | sh
```

```powershell
powershell -ExecutionPolicy ByPass -c "irm https://astral.sh/uv/install.ps1 | iex"
```

### 构建命令

```powershell
uv sync --frozen
uv run mkdocs build
```

### 升级依赖版本

```powershell
uv lock --upgrade <package-name>==<version>
```

## 许可证

本项目代码和文档分别采用不同的许可证，详情请参阅 [LICENSE](./LICENSE) 文件。

1. 代码采用 [Apache-2.0](./LICENSES/Apache-2.0.txt) 许可证
1. 文档采用 [CC BY-NC-ND 4.0](./LICENSES/CC-BY-NC-ND-4.0.txt) 许可证
