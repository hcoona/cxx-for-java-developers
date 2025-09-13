// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
using UnionStorage =
    absl::variant<absl::monostate /* for empty case */, uint64_t, std::string,
                  double, std::vector<MyClass>>;

UnionStorage s{3.14};
double* d = absl::get_if<double>(&s);
// --8<-- [end:code]
