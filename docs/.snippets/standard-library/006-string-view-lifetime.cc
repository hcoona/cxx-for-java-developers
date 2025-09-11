// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
absl::string_view str_view;
{
  std::string str = "Hello World!";

  str_view = str;
  LOG(INFO) << str_view;  // Valid here.

  str = "";
  LOG(INFO) << str_view;  // Invalid here!

  str_view = str;  // Valid again.
}
// Invalid again because |str| is destructed.
// --8<-- [end:code]
