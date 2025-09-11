// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
EXPECT_TRUE(std::make_reverse_iterator(it).base() == it);
EXPECT_TRUE(v.rbegin().base() == v.end());
EXPECT_TRUE(v.rend().base() == v.begin());
// --8<-- [end:code]
