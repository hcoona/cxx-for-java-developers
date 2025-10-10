/*
 * SPDX-FileCopyrightText: 2021 Shuai Zhang
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// --8<-- [start:code]
class ImplicitCastAllowedInt {
 public:
  ImplicitCastAllowedInt(int value);
};

class ImplicitCastDisallowedInt {
 public:
  explicit ImplicitCastDisallowedInt(int value);
};

void PassImplicitCastAllowedInt(ImplicitCastAllowedInt);
void PassImplicitCastDisallowedInt(ImplicitCastDisallowedInt);

PassImplicitCastAllowedInt(1);  // Implicit cast 1 to ImplicitCastAllowedInt(1)
PassImplicitCastDisallowedInt(1);  // Won't compile!
// --8<-- [end:code]
