<!--
SPDX-FileCopyrightText: 2021 Shuai Zhang

SPDX-License-Identifier: CC-BY-NC-ND-4.0
-->

# Packaging for Use by Other Languages

In general, you compile a `.so` (shared library) file that other languages can call through FFI. If you need to allow replacing the `.so` at runtime, you must keep the ABI stable. The simplest approach is to expose a C API. Another option is to use SWIG, but it is usually difficult to ensure ABI compatibility this way. Without ABI compatibility, you can't just swap the `.so`; you have to rebuild your project.
