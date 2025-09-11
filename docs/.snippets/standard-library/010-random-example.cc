// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
// Will be used to obtain a seed for the random number engine
std::random_device rd;

// Standard mersenne_twister_engine seeded with |rd()|.
std::mt19937 gen(rd());

// Produce uniform distribution for range [1, 6] (both boundary inclusive).
std::uniform_int_distribution<> distrib(1, 6);

v->reserve(n);
for (int i = 0; i < n; i++) {
  v->emplace_back(distrib(gen));
}
// --8<-- [end:code]
