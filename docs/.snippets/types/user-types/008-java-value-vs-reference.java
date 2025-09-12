// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
int a;
int b;
b = a; // Copy the value of `a` to `b`, both `a` and `b` have the same value.

Integer c;
Integer d;
d = c; // Copy the reference from `c` to `d`, both `c` and `d` reference the same value.
// --8<-- [end:code]
