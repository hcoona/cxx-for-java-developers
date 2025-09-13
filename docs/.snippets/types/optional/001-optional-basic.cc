// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
absl::optional<Pie> MakePie() {
  bool failed = /* ... */;
  if (failed) {
    return absl::nullopt;
  }

  return Pie();
}
// --8<-- [end:code]
