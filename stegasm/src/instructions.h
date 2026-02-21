//
// Created by Roumite on 20/02/2026.
//

#pragma once

#include <array>
#include <cstdint>
#include <string_view>

#include "interpreter/instructions/instruction.h"

enum RegNames
{
    R0 = 0b000,
    R1 = 0b001,
    R2 = 0b010,
    R3 = 0b011,
    R4 = 0b100,
    R5 = 0b101,
    R6 = 0b110,
    R7 = 0b111
};

enum RegCount
{
    NO_REG = 0,
    ONE_REG = 1,
    TWO_REG = 2,
    THREE_REG = 3,
    FOUR_REG = 4,
    FIVE_REG = 5,
    SIX_REG = 6
};

enum DataCount
{
    NO_DATA = 0,
    ONE_DATA = 1,
    TWO_DATA = 2
};

struct InstructionDesc
{
    const std::string_view name;
    uint8_t opcode;
    RegCount regCount;
    DataCount dataCount;
    InstructionFct fn;
};

constexpr std::array instructionSet =
{
    InstructionDesc{ "LOADA", 0x1, ONE_REG, ONE_DATA, &instr_LOADA }
};

#include "check_instructions.hpp"
#include "instruction_utils.hpp"
