// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
std::this_thread::sleep_for(std::chrono::milliseconds(kSleepMillis));

// Since c++14
std::this_thread::sleep_for(2000ms);

// Abseil
absl::SleepFor(absl::Milliseconds(kSleepMillis));
// --8<-- [end:code]
