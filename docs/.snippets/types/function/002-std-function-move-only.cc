// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
MoveOnlyInt v(1);
auto lambda = [v = std::move(v)]() { LOG(INFO) << v.value(); };
std::function<void()> f = [lambda = std::make_shared<decltype(lambda)>(
                               std::move(lambda))]() { lambda(); };
// --8<-- [end:code]
