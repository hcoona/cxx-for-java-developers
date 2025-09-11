// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
struct Record {
  std::string name;
  unsigned int floor;
  double weight;
  auto operator<=>(const Record&) const = default;
};
// records can now be compared with ==, !=, <, <=, >, and >=
// --8<-- [end:code]
