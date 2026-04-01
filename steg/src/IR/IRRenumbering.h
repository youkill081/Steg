//
// Created by Roumite on 31/03/2026.
//

#pragma once

#include "ir_structure.h"

#include <memory>
#include <vector>

namespace compiler
{
    class IRRenumbering
    {
    private:
        const std::vector<std::shared_ptr<IrBasicBlock>>& _src_blocks;

    public:
        IRRenumbering(
            const std::vector<std::shared_ptr<IrBasicBlock>>& src_blocks,
            const std::vector<IrGlobal>&
        ) : _src_blocks(src_blocks) {}

        void renumber() const
        {
            uint64_t counter = 0;
            for (auto& block : _src_blocks)
                for (auto& instr : block->instructions)
                    instr.instr_nbr = counter++;
        }
    };
}
