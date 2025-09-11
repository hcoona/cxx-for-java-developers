// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
inline bool operator==(const X& lhs, const X& rhs) { /* do actual comparison */
}
inline bool operator!=(const X& lhs, const X& rhs) { return !(lhs == rhs); }
// --8<-- [end:code]
