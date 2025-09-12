// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
class Person {
  //...
 private:
  static constexpr int32_t kUnspecifiedAge = -1;

  std::string name_{};            // Initialized with an empty string.
  int32_t age_{kUnspecifiedAge};  // Initialized with an constant.
};

// Defined in person.cc, prior to C++17.
constexpr int32_t Person::kUnspecifiedAge;
// --8<-- [end:code]
