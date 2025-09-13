// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
Factory& Factory::GetInstance() {
  static Factory instance;
  return instance;
}
// --8<-- [end:code]
