//
// Created by Roumite on 06/04/2026.
//

#pragma once

#include <memory>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ir_structure.h"

namespace compiler
{
    using ExecFn = uint32_t(*)(uint32_t, uint32_t);

    class IRConstantFolding
    {
    public:
        IRConstantFolding(
            const std::vector<std::shared_ptr<IrBasicBlock>>& src_blocks,
            const std::vector<IrGlobal>& src_globals);

        void fold();

    private:
        const std::vector<std::shared_ptr<IrBasicBlock>>& _src_blocks;
        const std::vector<IrGlobal>& _src_globals;

        std::unordered_map<std::string, uint32_t> _folded_values;
        std::string _current_function_name;

        static const std::unordered_map<IrOpCode, ExecFn> _binary_exec;
        static const std::unordered_map<IrOpCode, ExecFn> _unary_exec;

        void fold_block(IrBasicBlock& block);
        void fold_instruction(IrInstruction& instr);
        void propagate_operand(IrOperand& operand) const;

        void update_folded(const IrInstruction& instr);

        static std::unordered_set<std::string> collect_reads(const IrBasicBlock& block
        );
        static void eliminate_dead_copies(IrBasicBlock& block);

        static std::optional<uint32_t> try_convert(const std::string& value, IrValueType type);
        static std::string format_constant(uint32_t raw, IrValueType type);
    };
}
