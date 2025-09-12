// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
// !!! DON'T DO THIS !!!
constexpr std::string kHelloWorldMessage1 = "Hello World!";

// Do it this way.
constexpr char kHelloWorldMessage2[] = "Hello World!";

// In case you really need std::string or some complex types.
// constants.h
const std::string& GetHelloWorldMessage3();
// constants.cc
const std::string& GetHelloWorldMessage3() {
  static const std::string kHelloWorldMessage3 = "Hello World!";
  return kHelloWorldMessage3;
}
// --8<-- [end:code]
