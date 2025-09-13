// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
class HeaderOnlyClass {
  enum { kDefaultAnswer = 42 };

 public:
  // ...

 private:
  int answer_{kDefaultAnswer};
};
// --8<-- [end:code]
