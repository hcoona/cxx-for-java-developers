// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
constexpr int64_t kAnswer = 42;

void SayHello();

class Example {
 public:
  Example();
  virtual ~Example();

  int64_t IncThenGet();

 private:
  int64_t data_{kAnswer};
};

// The implementation of this method must be defined in the header file.
template <typename T>
void ignore_result(const T&) {}

// example.cc
void SayHello() { LOG(INFO) << "Hello!"; }

Example::Example() { LOG(INFO) << "Answer = " << data_; }

Example::~Example() { LOG(INFO) << "Answer = " << data_; }

int64_t Example::IncThenGet() { return ++data_; }
// --8<-- [end:code]
