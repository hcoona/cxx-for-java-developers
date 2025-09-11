// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
inline bool operator==(const X& lhs, const X& rhs) {
  return cmp(lhs, rhs) == 0;
}
inline bool operator!=(const X& lhs, const X& rhs) {
  return cmp(lhs, rhs) != 0;
}
inline bool operator<(const X& lhs, const X& rhs) { return cmp(lhs, rhs) < 0; }
inline bool operator>(const X& lhs, const X& rhs) { return cmp(lhs, rhs) > 0; }
inline bool operator<=(const X& lhs, const X& rhs) {
  return cmp(lhs, rhs) <= 0;
}
inline bool operator>=(const X& lhs, const X& rhs) {
  return cmp(lhs, rhs) >= 0;
}
// --8<-- [end:code]
