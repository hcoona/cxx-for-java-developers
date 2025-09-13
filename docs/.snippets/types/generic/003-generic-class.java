// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

import java.util.Iterator;

// --8<-- [start:code]
public interface List<E> {
    void add(E x);
    Iterator<E> iterator();
}
// --8<-- [end:code]
