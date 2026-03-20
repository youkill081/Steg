//
// Created by Roumite on 20/03/2026.
//

#pragma once

#include "ir_structure.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace compiler
{
    class IRLowering
    {
    public:
        std::vector<std::shared_ptr<IrBasicBlock>> lowered_blocks;
        std::vector<IrGlobal> lowered_globals;

        IRLowering(
            const std::vector<std::shared_ptr<IrBasicBlock>>& src_blocks,
            const std::vector<IrGlobal>& src_globals);

        void lower();
    private:
        const std::vector<std::shared_ptr<IrBasicBlock>>& _src_blocks;
        const std::vector<IrGlobal>& _src_globals;

        std::unordered_set<std::string> _global_names;
        std::unordered_map<std::string, IrValueType> _global_types;
        std::unordered_map<std::string, std::string> _ptr_local_to_global;

        std::string _current_function_name;
        uint64_t _temp_count = 0;

        std::string new_temp();
        static bool is_user_variable(const std::string& name);
        static IrOpCode load_opcode(IrValueType t);
        static IrOpCode store_opcode(IrValueType t);

        IrOperand lower_src(const IrOperand& op, std::vector<IrInstruction>& out);

        void lower_instruction(
            const IrInstruction& instr,
            std::vector<IrInstruction>& out);

        void lower_block(
            const IrBasicBlock& src,
            IrBasicBlock& dst,
            std::vector<IrInstruction>& instr_out);
    };
}
