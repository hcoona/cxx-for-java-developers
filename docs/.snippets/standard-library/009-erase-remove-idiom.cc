// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
std::vector<int> v = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
v.erase(std::remove_if(v.begin(), v.end(), IsOdd), v.end());
// --8<-- [end:code]
