// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
class IntType {
 public:
  explicit IntType(int v);
  // ...
};

double d = 7.9;
IntType i1(d);  // bad: narrowing
IntType i2{d};  // Won't compile!
int i = d;      // bad: narrowing
int i{d};       // Won't compile!
// --8<-- [end:code]
