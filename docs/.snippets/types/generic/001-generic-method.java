// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
public <T> java.util.List<T> fromArrayToList(T[] a) {
    return java.util.Arrays.stream(a).collect(java.util.stream.Collectors.toList());
}
// --8<-- [end:code]
