/*
 * SPDX-FileCopyrightText: 2021 Shuai Zhang
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// --8<-- [start:code]
class Person {
 public:
  Person() = default;

  // !!! THIS IS AN INCORRECT PRACTICE !!!
  Person(std::string name)
      :  // Expecting |name| valid here, but actually not! UNDEFINED BEHAVIOR!
        id_(absl::StrCat(name, "_", GetNextUniqueId())),
        // The next line would be executed before initializing |id_|.
        name_(std::move(name)) {}

 private:
  std::string name_;
  std::string id_;
};
// --8<-- [end:code]
