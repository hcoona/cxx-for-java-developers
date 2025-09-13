// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
Consumer<String> c = s -> System.out.println(s);
Predicate<String> p = s -> s.isEmpty();
// Function, BiFunction, ...
// --8<-- [end:code]
