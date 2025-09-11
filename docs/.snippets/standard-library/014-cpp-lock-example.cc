// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
{
  std::lock_guard lock(mutex);  // Locking when |lock| creating.
  // locked here.
}  // Auto unlock when |lock| destructing

{
  absl::MutexLock(&mutex);
  // ...
}
// --8<-- [end:code]
