// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
lock.lock();  // block until condition holds
try {
    // ... method body
} finally {
    lock.unlock();
}
// --8<-- [end:code]
