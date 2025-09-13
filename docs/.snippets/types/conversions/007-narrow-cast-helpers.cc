// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
double d = 7.9;

// If you included GSL in your project
// https://github.com/microsoft/GSL
int i = gsl::narrow_cast<int>(d);

// Make your version of narrow_cast
// narrow_cast(): a searchable way to do narrowing casts of values
template <class T, class U>
constexpr T narrow_cast(U&& u) noexcept {
  return static_cast<T>(std::forward<U>(u));
}

// static_cast if neither include GSL nor make your version of narrow_cast
int i2 = static_cast<int>(d);
// --8<-- [end:code]
