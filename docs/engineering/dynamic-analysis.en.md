<!--
SPDX-FileCopyrightText: 2021 Shuai Zhang

SPDX-License-Identifier: CC-BY-NC-ND-4.0
-->

# Dynamic Analysis

This section lists several common tools. Detailed content hasn't been written yet, so it's temporarily left blank.

## Memory Leaks

### valgrind

### jemalloc/tcmalloc

## Sanitizers

<https://github.com/google/sanitizers/wiki>

### AddressSanitizer (ASAN)

### ThreadSanitizer (TSAN)

### MemorySanitizer (MSAN)

## Fuzzing

/// admonition | Note
It's strongly recommended to perform fuzzing tests on public APIs that expose a bytes interface.
///

<https://www.wikiwand.com/en/Fuzzing>
