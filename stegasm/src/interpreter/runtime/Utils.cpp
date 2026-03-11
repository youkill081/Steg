//
// Created by Roumite on 23/02/2026.
//

#include "Utils.h"

#include "Runtime.h"

std::string Utils::get_string_from_address(const uint16_t address) const
{
    std::string end;
    for (uint16_t i = address; _runtime.memory.read_uint8(i) != '\0'; i++)
        end += _runtime.memory.read_uint8(i);
    return end;
}
