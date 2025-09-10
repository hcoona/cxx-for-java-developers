<!--
SPDX-FileCopyrightText: 2021 Shuai Zhang

SPDX-License-Identifier: CC-BY-NC-ND-4.0
-->

# 动态分析

这里就列了一些常见工具，之前就没有写具体内容，暂时留空。

## 内存泄漏

### valgrind

### jemalloc/tcmalloc 等

## Sanitizer

<https://github.com/google/sanitizers/wiki>

### AddressSanitizer (ASAN)

### ThreadSanitizer (TSAN)

### MemorySanitizer (MSAN)

## Fuzzing

/// admonition | 注意
强烈建议对以 bytes 为接口的 public API 做 fuzzing 测试。
///

<https://www.wikiwand.com/en/Fuzzing>
