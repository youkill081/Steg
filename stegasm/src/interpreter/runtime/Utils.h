//
// Created by Roumite on 23/02/2026.
//

#pragma once

#include <string>
#include <cstdint>

class Runtime;

class Utils
{
private:
    Runtime &_runtime;
public:
    explicit Utils(Runtime &runtime) : _runtime(runtime) {}

    std::string get_string_from_address(uint16_t address) const;
};
