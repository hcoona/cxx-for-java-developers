// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
std::function<void(absl::string_view)> c = [](absl::string_view s) {
  LOG(INFO) << s;
};
std::function<bool(absl::string_view)> p = [](absl::string_view s) {
  return s.empty();
};

std::function<Status(int64_t /* id */, absl::string_view /* name */, Gender)>
    f = [](int64_t id, absl::string_view name, Gender gender) {
      // ...
    };
// --8<-- [end:code]
