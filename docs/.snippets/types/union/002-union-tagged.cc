// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
enum class UnionStorageDataType {
  kUnspecified = 0,
  kInt32 = 1,
  kUint16Array = 2,
  kUint8 = 3,
};

struct UnionStorage {
  UnionStorageDataType data_type;
  union {
    int32_t n;
    uint16_t s[2];
    uint8_t c;
  } data;
};
// --8<-- [end:code]
