// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
using Orange = int;
using Apple = int;

Apple apple(2);
Orange orange = apple;      // Orange should not be able to become an Apple.
Orange x = orange + apple;  // Shouldn't add Oranges and Apples.
if (orange > apple)
  ;  // Shouldn't compare Apples to Oranges.

void foo(Orange);
void foo(Apple);  // Redefinition.
// --8<-- [end:code]
