// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
// 基本的 old-style 与 scoped-enum 对比示例
// Deprecated style.
enum Fruit {
  FRUIT_UNSPECIFIED = 0,
  FRUIT_APPLE = 1,
};

// |FRUIT_UNSPECIFIED| is visible.

// Suggested style. The only difference is |class| keyword.
enum class FruitNew {
  kUnspecified = 0,
  kApple = 1,
};

// |kUnspecified| is invisible. You need to reference it as
// |FruitNew::kUnspecified|.
// --8<-- [end:code]
