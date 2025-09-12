// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
// Loop never end because |unsigned int| would wrap to |UINT_MAX| when counting
// down beyond 0.
for (auto i = v.size(); i >= 0; --i) {
}
// --8<-- [end:code]
