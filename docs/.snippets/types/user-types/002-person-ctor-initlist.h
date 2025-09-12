// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
class Person {
 public:
  Person() = default;  // Use default keyword to generate default constructor.
                       // Provide a default constructor is useful for receiving
                       // values from output parameters.
                       //
                       // Example:
                       //   Person p;
                       //   Status s = LoadPerson(db, key, &p);
                       //   CHECK(s.ok()) << "Failed to load. reason=" << s;

  // Although |name_| & |age_| default value is specified during declaration,
  // the member initializer list would override it when using this constructor.
  Person(std::string name, int32_t age) : name_(std::move(name)), age_(age) {
    // Execute constructor body after member initializer list executed.
    CHECK(!name_.empty());  // Cannot use |name| here because it already moved.
    CHECK_GT(age_, 0);
  }

 private:
  static constexpr int32_t kUnspecifiedAge = -1;

  std::string name_{};            // Initialized with an empty string.
  int32_t age_{kUnspecifiedAge};  // Initialized with an constant.
};

// Defined in person.cc, prior to C++17.
constexpr int32_t Person::kUnspecifiedAge;
// --8<-- [end:code]
