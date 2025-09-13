// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
int OpenReadFile1(absl::string_view filename) {
  // Must copy it to append \0 at the end.
  std::string copied_filename(filename);
  return ::open(copied_filename.c_str(), O_RDONLY);
}
// --8<-- [end:code]
