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
    static uint64_t _str_count = 0;

    class IRLowering
    {
    public:
        std::unordered_map<std::string, uint8_t> _global_ptr_depths;
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
        std::unordered_map<std::string, std::string> _string_constants;
        std::unordered_map<std::string, std::string> _address_taken; // "fn_label::var_name" -> "mangled_global_name"

        std::string _current_function_name;
        uint64_t _temp_count = 0;

        /* move local variable given as address in globals, mangle the name */
        void collect_address_taken(const IrBasicBlock& entry, size_t start_index);
        std::string mangle_local(const std::string& fn, const std::string& var);
        [[nodiscard]] bool is_address_taken(const std::string& var) const;
        [[nodiscard]] std::string remap_name(const std::string& var) const;

        std::string new_temp();
        static IrOpCode load_opcode(IrValueType t);
        static IrOpCode store_opcode(IrValueType t);

        IrOperand lower_src(const IrOperand& op, std::vector<IrInstruction>& out);
        IrValueType effective_type(IrValueType vt, const std::string& name) const;

        void lower_instruction(
            const IrInstruction& instr,
            std::vector<IrInstruction>& out);

        void lower_block(
            const IrBasicBlock& src,
            IrBasicBlock& dst,
            std::vector<IrInstruction>& instr_out);
    };
}
