// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
class Point {
 public:
  bool operator<(Point&);  // Declare a member operator overload.

  // Declare addition operators.
  friend Point operator+(Point&, int);
  friend Point operator+(int, Point&);
};

// Declare a global operator overload.
bool operator==(const Point& lhs, const Point& rhs);
// --8<-- [end:code]
