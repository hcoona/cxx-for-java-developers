// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
auto start = std::chrono::steady_clock::now();
LOG(INFO) << "f(42) = " << fibonacci(42);
auto end = std::chrono::steady_clock::now();

std::chrono::duration<double> elapsed = end - start;
LOG(INFO) << "elapsed time: "
          << std::chrono::duration_cast<std::chrono::seconds>(elapsed).count()
          << "s\n";
// --8<-- [end:code]
