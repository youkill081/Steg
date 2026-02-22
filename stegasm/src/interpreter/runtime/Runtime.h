//
// Created by Roumite on 20/02/2026.
//

#pragma once

#include <vector>

#include "Memory.h"
#include "Registries.h"

using ComparisonFlag = struct ComparisonFlag
{
    bool equal = false;
    bool greater = false;
    bool lower = false;
};

class Runtime
{
public:
    Memory memory;
    Registries registries;
    ComparisonFlag comparison_flag;

    std::vector<uint64_t> instructions{};
    uint64_t instruction_pointer = 0;
    bool is_running = true;

    uint16_t return_value = 0;
};
