// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
class Circle {
 public:
  // ...

  template <typename H>
  friend H AbslHashValue(H h, const Circle& c) {
    return H::combine(std::move(h), c.center_, c.radius_);
  }

  // ...

 private:
  std::pair<int, int> center_;
  int radius_;
};

// Use it in unordered_map as Key type.
std::unordered_map<Circle, MyValue, absl::Hash<Circle>> my_map;
// --8<-- [end:code]
