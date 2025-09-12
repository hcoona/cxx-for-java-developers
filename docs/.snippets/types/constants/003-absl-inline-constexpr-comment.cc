// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
// Macro: ABSL_INTERNAL_INLINE_CONSTEXPR(type, name, init)
//
// Description:
//   Expands to the equivalent of an inline constexpr instance of the specified
//   `type` and `name`, initialized to the value `init`. If the compiler being
//   used is detected as supporting actual inline variables as a language
//   feature, then the macro expands to an actual inline variable definition.
//
// Requires:
//   `type` is a type that is usable in an extern variable declaration.
//
// Requires: `name` is a valid identifier
//
// Requires:
//   `init` is an expression that can be used in the following definition:
//     constexpr type name = init;
//
// Usage:
//
//   // Equivalent to: `inline constexpr size_t variant_npos = -1;`
//   ABSL_INTERNAL_INLINE_CONSTEXPR(size_t, variant_npos, -1);
//
// Differences in implementation:
//   For a direct, language-level inline variable, decltype(name) will be the
//   type that was specified along with const qualification, whereas for
//   emulated inline variables, decltype(name) may be different (in practice
//   it will likely be a reference type).
// --8<-- [end:code]
