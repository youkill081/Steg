//
// Created by Roumite on 20/02/2026.
//

#pragma once

#include <stack>
#include <vector>

#include "Memory.h"
#include "Registries.h"

using ComparisonFlag = struct ComparisonFlag
{
    bool equal = false;
    bool greater = false;
    bool lower = false;
};

using LoadedInstruction = struct LoadedInstruction
{
    InstructionView view;
    const InstructionDesc &desc;
};

class Runtime
{
public:
    Memory memory;
    Registries registries;
    ComparisonFlag comparison_flag;

    std::vector<LoadedInstruction> instructions{};
    std::stack<uint16_t> stack{};
    uint64_t instruction_pointer = 0;
    bool is_running = true;

    uint16_t return_value = 0;
};
