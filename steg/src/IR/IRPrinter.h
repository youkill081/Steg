//
// Created by Roumite on 19/03/2026.
//

#pragma once

#include "ir_structure.h"
#include "IRGenerator.h"

#include <string>
#include <vector>
#include <memory>

namespace compiler
{
    class IRPrinter
    {
        const std::vector<std::shared_ptr<IrBasicBlock>>& _blocks;
        const std::vector<IrGlobal>& _globals;
        const std::vector<IrFile>& _files;

    public:
        explicit IRPrinter(
            const std::vector<std::shared_ptr<IrBasicBlock>>& blocks,
            const std::vector<IrGlobal>& globals,
            const std::vector<IrFile>& files
        ) : _blocks(blocks), _globals(globals), _files(files)
        {
        }

        explicit IRPrinter(const IRGenerator& gen)
            : _blocks(gen.all_blocks), _globals(gen.globals), _files(gen.files)
        {
        }

        [[nodiscard]] std::string print() const;

    private:
        static std::string format_operand(const IrOperand& op);
        static std::string format_value_type(const IrValueType& value_type, uint32_t ptr_depth);
        static std::string format_instruction(const IrInstruction& i);
        static std::string format_terminator(const IrBasicBlock& block);
    };
}
