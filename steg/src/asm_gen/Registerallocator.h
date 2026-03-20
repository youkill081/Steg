//
// Created by Roumite on 20/03/2026.
//
//
// Created by Roumite on 20/03/2026.
//

#pragma once

#include "IR/ir_structure.h"

#include <array>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace compiler
{
    static constexpr int k_reg_return      = 0;
    static constexpr int k_reg_param_first = 1;
    static constexpr int k_reg_param_last  = 6;
    static constexpr int k_reg_free_first  = 7;
    static constexpr int k_reg_free_last   = 31;
    static constexpr int k_reg_free_count  = k_reg_free_last - k_reg_free_first + 1; // 25

    inline std::string reg_name(int index)
    {
        return "R" + std::to_string(index);
    }

    inline std::string spill_name(int index)
    {
        return "GLOB_REGISTRY_R" + std::to_string(index);
    }

    struct RegisterAllocation
    {
        std::unordered_map<std::string, std::string> reg_map;
        std::vector<IrGlobal> spill_globals;

        [[nodiscard]] std::string lookup(
            const std::string& function_label,
            const std::string& temp_name) const;

        [[nodiscard]] static bool is_spill(const std::string& name);
    };

    class RegisterAllocator
    {
    public:
        explicit RegisterAllocator(
            const std::vector<std::shared_ptr<IrBasicBlock>>& blocks,
            const std::unordered_set<std::string>&             global_names);

        [[nodiscard]] RegisterAllocation allocate();
    private:
        const std::vector<std::shared_ptr<IrBasicBlock>>& _blocks;
        const std::unordered_set<std::string>&             _global_names;

        std::string _current_function;
        std::vector<int> _free_regs;

        int _next_spill_index = k_reg_free_last + 1;

        RegisterAllocation _result;

        [[nodiscard]] std::string make_key(const std::string& temp_name) const;
        [[nodiscard]] bool needs_allocation(const IrOperand& op) const;
        void ensure_allocated(const std::string& temp_name);
        void reset_free_pool();
        void process_instruction(const IrInstruction& instr);
    };
}