// SPDX-FileCopyrightText: 2021 Shuai Zhang
//
// SPDX-License-Identifier: Apache-2.0

// --8<-- [start:code]
std::shared_ptr<Animal> animal_dog = MakeDog();
std::shared_ptr<Dog> dog = std::static_pointer_cast<Dog>(animal_dog);
// --8<-- [end:code]
