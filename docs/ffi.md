<!--
SPDX-FileCopyrightText: 2021 Shuai Zhang

SPDX-License-Identifier: CC-BY-NC-ND-4.0
-->

# 打包提供给其他语言使用

大致上来说就是编译出一个 `.so` 文件出来给其他语言通过 FFI 调用，如果需要考虑运行时替换 `.so` 文件的话，需要保持 ABI 兼容性。最简单的做法就是转换成 C API 暴露出去。还有别的办法就是通过 SWIG，但是基本上很难保证 ABI 兼容。不保证 ABI 兼容意味着你不能只替换 `.so` 文件，必须重新编译你的项目。
