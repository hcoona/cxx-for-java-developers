// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
int a = 3;  // Create on stack.

// Always use smart pointers, never use `new` keyword & `delete` keyword.
// Don't panic. Would talk about it in following sections.
// Check following documents for further details:
// * https://en.cppreference.com/w/cpp/memory
// * https://www.stroustrup.com/C++11FAQ.html#std-unique_ptr
auto b =
    std::make_unique<Integer>(5);  // Create on heap & forbid ownership sharing.
auto c =
    std::make_shared<Integer>(7);  // Create on heap & allow ownership sharing.
// --8<-- [end:code]
