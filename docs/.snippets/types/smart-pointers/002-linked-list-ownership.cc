// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
std::vector<std::unique_ptr<LinkedNode>> nodes;
LinkedNode* head;

auto c = std::make_unique<LinkedNode>("c", /* next */ nullptr);
auto b = std::make_unique<LinkedNode>("b", /* next */ c.get());
auto a = std::make_unique<LinkedNode>("a", /* next */ b.get());
c->set_next(a.get());

head = a.get();

nodes.emplace_back(std::move(c));
nodes.emplace_back(std::move(b));
nodes.emplace_back(std::move(a));
// --8<-- [end:code]
