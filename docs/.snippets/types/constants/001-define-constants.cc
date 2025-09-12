// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
constexpr int32_t kTwo = 2;
const int32_t kTwo2 = 2;  // NOTE: original text had a typo 'in32_t'.

enum : int32_t {  // or any other necessary integral type.
  kTwo3 = 2,
};

// Only take the following pattern as last resort.
#define YOUR_PROJECT_PREFIX_TWO 2
// --8<-- [end:code]
