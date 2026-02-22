//
// Created by Roumite on 21/02/2026.
//

#pragma once

#include <array>

#include "../../instructions.h"

class Registries
{
private:
    static void check_registry_valid(uint16_t registry_number);

    std::array<uint16_t, number_of_registries> registries{};
public:
    void write(RegNames registry_name, uint16_t value);
    void write(uint16_t registry_number, uint16_t value);

    [[nodiscard]] uint16_t read(RegNames registry_name) const;
    [[nodiscard]] uint16_t read(uint16_t registry_number) const;

    void display() const;
};
