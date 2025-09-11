// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
int F(const std::unordered_map<int, int>& m, int t) {
  auto it = m.find(t);
  if (it == m.end()) {
    return -1;
  }

  return it->second;
}
// --8<-- [end:code]
