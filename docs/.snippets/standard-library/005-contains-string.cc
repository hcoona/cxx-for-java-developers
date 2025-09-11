// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
bool Contains(const std::string& str, char target) {
  auto idx = str.find(target);
  return idx != std::string::npos;
}
// --8<-- [end:code]
