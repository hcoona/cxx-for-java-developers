/*
 * SPDX-FileCopyrightText: 2021 Shuai Zhang
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// --8<-- [start:code]
// We can copy either the value or the reference of a ValueType.
//
// This is a trivial type. The compiler would generate the default constructor,
// default copy constructor & copy assignment operator, default move constructor
// & move assignment operator, default destructor for it. As a result, it's
// copyable & moveable.
//
// Example:
//   ValueType a;
//   ValueType b = a;   // Allowed to copy the value.
//   ValueType& c = a;  // Allowed to reference the value.
struct ValueType {
  int32_t id;
  std::string name;
};

// We cannot copy the value of a RefrenceType instance.
// We can only copy the reference to a ReferenceType instance.
//
// Example:
//   ReferenceType a(2, "mock_name");
//   ReferenceType b = a;   // !!!DISALLOW!!! Won't compile!
//   ReferenceType& b = a;  // Allowed to reference a ReferenceType instance.
class ReferenceType {
 public:
  ReferenceType(int32_t id, std::string name)
      : id_(id), name_(std::move(name)) {}
  // Make the destructor virtual to allow children override it.
  virtual ~ReferenceType() = default;

  // Disallow copy
  ReferenceType(const ReferenceType&) = delete;
  ReferenceType& operator=(const ReferenceType&) = delete;

  // Allow move, move means steal the content of `other` instance.
  // Don't panic. Would talk about move in following chapters.
  ReferenceType(ReferenceType&& other)
      : id_(other.id_), name_(std::move(other.name_)) {
    other.id_ = 0;
  }
  ReferenceType& operator=(ReferenceType&& other) {
    id_ = other.id_;               // Cannot move int, it's a primitive type.
    other.id_ = 0;                 // Copy then clear the field.
    name_ = std::move(other.name_);  // Move std::string
  }
};
// --8<-- [end:code]
