// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
AtomicLong* const a = new AtomicLong(3);  // Equal to the Java example,
                                          // The const is against the pointer.

const AtomicLong* b =
    new AtomicLong(5);  // Disallow to modify the internal value of b,
                        // but allow to assign b to a new pointer.
                        // The const is against AtomicLong type.

// Just for example
class AtomicLong {
 public:
  int64_t value() const;          // This is a read-only method, visible for an
                                  // AtomicLong const type.
  void set_value(int64_t value);  // This is not a read-only method, invisible
                                  // for an AtomicLong const type.

 private:
  int64_t value_;
};
// --8<-- [end:code]
