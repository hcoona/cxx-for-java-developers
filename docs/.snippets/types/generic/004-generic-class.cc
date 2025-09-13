// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
template <typename T>
class List {
 public:
  virtual ~List() = default;

  virtual void Add(T element) = 0;
  template <typename Iterator>
  Iterator iterator() const;  // Placeholder; real Iterator type omitted.
};
// --8<-- [end:code]
