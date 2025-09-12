// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
// Deprecated
typedef int int32_t;

// Suggest
using int32_t = int;

// Template
template <typename T>
using MyArray = std::vector<T>;
// --8<-- [end:code]
