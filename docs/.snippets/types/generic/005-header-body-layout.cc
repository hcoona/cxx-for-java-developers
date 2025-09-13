// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
// vector.h
template <typename T>
class FixedArray {
 public:
  // omitted: constructor, destructor, copy/move, etc.

  void resize(int32_t new_size);

 private:
  std::unique_ptr<T[]> data_;
  int32_t size_;
};

#include "vector.inc"  // NOLINT

// vector.inc
template <typename T>
void FixedArray<T>::resize(int32_t new_size) {
  // ...
}
// --8<-- [end:code]
