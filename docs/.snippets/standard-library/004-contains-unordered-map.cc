// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
bool Contains(const std::unordered_map<int, int>& map, int target) {
  auto it = map.find(target);
  // end() pos means not found.
  return it != map.end();
}
// --8<-- [end:code]
