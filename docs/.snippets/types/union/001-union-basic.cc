// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
union UnionStorage {
  int32_t n;      // occupies 4 bytes
  uint16_t s[2];  // occupies 4 bytes
  uint8_t c;      // occupies 1 byte
};  // the whole union occupies 4 bytes
// --8<-- [end:code]
