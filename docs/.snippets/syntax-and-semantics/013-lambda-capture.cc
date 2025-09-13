// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
int a;
int b;
std::unique_ptr<int> c;
int d;

// Take care that the lambda need to be called while |b| is still alive.
auto lambda = [a /* copy a */, &b /* reference b */,
               c = std::move(c) /* calculated result */](
                  /* parameters */) {
  // you cannot use |d| because you didn't capture it.
  // ensure the reference captured variables are still living.
};
// --8<-- [end:code]
