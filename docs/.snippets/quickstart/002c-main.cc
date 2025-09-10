// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
// example/main.cc
#include <iostream>

#include "example/example_lib.h"

int main() {
  int a;
  int b;
  std::cin >> a >> b;
  std::cout << example::sum(a, b) << std::endl;
}
// --8<-- [end:code]
