//
// Created by Roumite on 20/02/2026.
//

#pragma once

#include "../Runtime.h"

using DecodedInstruction = struct DecodedInstruction
{
    uint8_t opcode;
    uint8_t regIndex[6];
    uint16_t data[2];
};

using InstructionFct = void(*)(Runtime &, DecodedInstruction &);

inline void instr_LOADA(Runtime &, DecodedInstruction &) {};