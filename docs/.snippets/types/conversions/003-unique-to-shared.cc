// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
std::unique_ptr<Animal> MakeAnimal();

std::shared_ptr<Animal> animal = MakeAnimal();

std::unique_ptr<Animal> uniq_animal = MakeAnimal();
std::shared_ptr<Animal> shared_animal = std::move(uniq_animal);
// --8<-- [end:code]
