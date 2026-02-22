//
// Created by Roumite on 20/02/2026.
//

#pragma once

#include "../runtime/Runtime.h"

struct InstructionDesc;

using DecodedInstruction = struct DecodedInstruction
{
    InstructionDesc &desc;
};

using InstructionFct = void(*)(Runtime &, DecodedInstruction &);

inline void instr_LOADA(Runtime &, DecodedInstruction &) {};