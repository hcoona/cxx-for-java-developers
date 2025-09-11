// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
class MyIntCounter {
 public:
  int32_t value() const;

 private:
  mutable absl::Mutex mutex_;
  // See static analysing thread-safety errors for further details about
  // ABSL_GUARDED_BY. Intrduced in the following subsection.
  int32_t value_ ABSL_GUARDED_BY(mutex_);
};

int32_t MyIntCounter::value() const {
  // clang++ with -Wthread-safety would report error if we access |value_|
  // without locking the |mutex_.| See
  // https://releases.llvm.org/8.0.1/tools/clang/docs/ThreadSafetyAnalysis.html
  absl::MutexLock lock(&mutex_);
  return value_;
}
// --8<-- [end:code]
