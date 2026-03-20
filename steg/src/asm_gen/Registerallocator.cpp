//
// Created by Roumite on 20/03/2026.
//

#include "RegisterAllocator.h"

using namespace compiler;

// ─────────────────────────────────────────────────────────────────────────────
//  RegisterAllocation helpers
// ─────────────────────────────────────────────────────────────────────────────

std::string RegisterAllocation::lookup(
    const std::string& function_label,
    const std::string& temp_name) const
{
    const auto key = function_label + "::" + temp_name;
    const auto it  = reg_map.find(key);
    return (it != reg_map.end()) ? it->second : "";
}

bool RegisterAllocation::is_spill(const std::string& name)
{
    // All spill names start with "GLOB_REGISTRY_R"
    static constexpr std::string_view prefix = "GLOB_REGISTRY_R";
    return name.rfind(prefix, 0) == 0;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Construction
// ─────────────────────────────────────────────────────────────────────────────

RegisterAllocator::RegisterAllocator(
    const std::vector<std::shared_ptr<IrBasicBlock>>& blocks,
    const std::unordered_set<std::string>&             global_names)
    : _blocks(blocks)
    , _global_names(global_names)
{
}

// ─────────────────────────────────────────────────────────────────────────────
//  Private helpers
// ─────────────────────────────────────────────────────────────────────────────

std::string RegisterAllocator::make_key(const std::string& temp_name) const
{
    return _current_function + "::" + temp_name;
}

bool RegisterAllocator::needs_allocation(const IrOperand& op) const
{
    if (op.type != IrOperandType::Temporary) return false; // Constants / labels
    if (op.value.empty())                    return false; // Empty result slot
    if (_global_names.count(op.value))       return false; // Lives in memory
    return true;
}

void RegisterAllocator::reset_free_pool()
{
    _free_regs.clear();
    _free_regs.reserve(k_reg_free_count);
    // Fill R7 … R31 in order so that lower-numbered registers are preferred.
    for (int r = k_reg_free_first; r <= k_reg_free_last; ++r)
        _free_regs.push_back(r);
}

void RegisterAllocator::ensure_allocated(const std::string& temp_name)
{
    const std::string key = make_key(temp_name);
    if (_result.reg_map.count(key))
        return; // Already assigned in an earlier instruction

    std::string assigned;

    if (!_free_regs.empty())
    {
        // Take the lowest-indexed available register.
        assigned = reg_name(_free_regs.front());
        _free_regs.erase(_free_regs.begin());
    }
    else
    {
        // No physical register available -> spill to a new global slot.
        assigned = spill_name(_next_spill_index);

        IrGlobal spill_global;
        spill_global.name  = assigned;
        spill_global.type  = IrValueType::UINT32; // DD – 32-bit slot
        // initial_value left empty (zero-initialised by the runtime)
        _result.spill_globals.push_back(std::move(spill_global));

        ++_next_spill_index;
    }

    _result.reg_map[key] = std::move(assigned);
}

void RegisterAllocator::process_instruction(const IrInstruction& instr)
{
    // ── Result operand ────────────────────────────────────────────────────────
    if (needs_allocation(instr.result))
        ensure_allocated(instr.result.value);

    // ── Source operands ───────────────────────────────────────────────────────
    // A source temp that has no prior definition (e.g. a user-declared variable
    // used before any assignment in this linear order) still needs a home.
    if (needs_allocation(instr.arg1))
        ensure_allocated(instr.arg1.value);

    if (needs_allocation(instr.arg2))
        ensure_allocated(instr.arg2.value);

    // ── Call arguments ────────────────────────────────────────────────────────
    for (const auto& arg : instr.call_args)
        if (needs_allocation(arg))
            ensure_allocated(arg.value);

    // ── Terminator operands are handled at the block level (see allocate()) ───
}

// ─────────────────────────────────────────────────────────────────────────────
//  Public entry point
// ─────────────────────────────────────────────────────────────────────────────

RegisterAllocation RegisterAllocator::allocate()
{
    for (const auto& block_ptr : _blocks)
    {
        const IrBasicBlock& block = *block_ptr;

        // ── Function-entry: start a fresh register pool ───────────────────────
        if (block.is_function_entry)
        {
            _current_function = block.function_name;
            reset_free_pool();
        }

        // ── Instructions ─────────────────────────────────────────────────────
        for (const auto& instr : block.instructions)
            process_instruction(instr);

        // ── Terminator operands ───────────────────────────────────────────────
        // BRANCH condition and RETURN value may reference temps that were not
        // seen as a result earlier (e.g. a bare global-load result used directly).
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