//
// Created by Roumite on 20/03/2026.
//

#include "RegisterAllocator.h"

using namespace compiler;

std::string RegisterAllocation::lookup(
    const std::string& function_label,
    const std::string& temp_name) const
{
    const auto key = function_label + "::" + temp_name;
    const auto it = reg_map.find(key);
    return (it != reg_map.end()) ? it->second : "";
}

bool RegisterAllocation::is_spill(const std::string& name)
{
    static constexpr std::string_view prefix = "GLOB_REGISTRY_R";
    return name.rfind(prefix, 0) == 0;
}

RegisterAllocator::RegisterAllocator(
    const std::vector<std::shared_ptr<IrBasicBlock>>& blocks,
    const std::unordered_set<std::string>& global_names)
    : _blocks(blocks)
      , _global_names(global_names)
{
}


std::string RegisterAllocator::make_key(const std::string& temp_name) const
{
    return _current_function + "::" + temp_name;
}

bool RegisterAllocator::needs_allocation(const IrOperand& op) const
{
    if (op.type != IrOperandType::Temporary) return false;
    if (op.value.empty()) return false;
    if (_global_names.count(op.value)) return false;
    return true;
}

void RegisterAllocator::reset_free_pool()
{
    _free_regs.clear();
    _free_regs.reserve(k_reg_free_count);
    for (int r = k_reg_free_first; r <= k_reg_free_last; ++r)
        _free_regs.push_back(r);
}

void RegisterAllocator::ensure_allocated(const std::string& temp_name)
{
    const std::string key = make_key(temp_name);
    if (_result.reg_map.count(key))
        return;

    std::string assigned;

    if (!_free_regs.empty())
    {
        assigned = reg_name(_free_regs.front());
        _free_regs.erase(_free_regs.begin());
    }
    else
    {
        assigned = spill_name(_next_spill_index);

        IrGlobal spill_global;
        spill_global.name = assigned;
        spill_global.type = IrValueType::UINT32;

        _result.spill_globals.push_back(std::move(spill_global));

        ++_next_spill_index;
    }

    _result.reg_map[key] = std::move(assigned);
}

void RegisterAllocator::process_instruction(const IrInstruction& instr)
{
    if (needs_allocation(instr.result))
        ensure_allocated(instr.result.value);

    if (needs_allocation(instr.arg1))
        ensure_allocated(instr.arg1.value);

    if (needs_allocation(instr.arg2))
        ensure_allocated(instr.arg2.value);

    for (const auto& arg : instr.call_args)
        if (needs_allocation(arg))
            ensure_allocated(arg.value);
}

RegisterAllocation RegisterAllocator::allocate()
{
    for (const auto& block_ptr : _blocks)
    {
        const IrBasicBlock& block = *block_ptr;

        if (block.is_function_entry)
        {
            _current_function = block.function_name;
            reset_free_pool();

            for (std::size_t i = 0; i < block.parameters.size() && i < 6; ++i)
            {
                const std::string& param_name = block.parameters[i];
                const std::string key = make_key(param_name);
                const std::string reg = "R" + std::to_string(static_cast<int>(i) + 1);

                _result.reg_map[key] = reg;

                _free_regs.erase(
                    std::remove(_free_regs.begin(), _free_regs.end(),
                                k_reg_free_first + static_cast<int>(i) - (k_reg_free_first - 1)),
                    _free_regs.end());
            }

        }

        for (const auto& instr : block.instructions)
            process_instruction(instr);

        if (block.terminator == IrBlockTerminator::BRANCH
            && needs_allocation(block.condition_operand))
        {
            ensure_allocated(block.condition_operand.value);
        }

        if (block.terminator == IrBlockTerminator::RETURN
            && needs_allocation(block.return_operand))
        {
            ensure_allocated(block.return_operand.value);
        }
    }

    return std::move(_result);
}
