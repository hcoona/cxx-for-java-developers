// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
template <typename T>
std::vector<T> fromArrayToList(const T* arr, int64_t size) {
  return std::vector<T>(arr, arr + size);
}
// --8<-- [end:code]
