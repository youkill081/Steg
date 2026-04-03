//
// Created by Roumite on 20/03/2026.
//

#include "Registerallocator.h"
#include "linter/Linter.h"

#include <algorithm>

using namespace compiler;

std::string RegisterAllocation::lookup(
    const std::string& function_label,
    const std::string& temp_name) const
{
    const auto key = function_label + "::" + temp_name;
    const auto it = reg_map.find(key);
    return (it != reg_map.end()) ? it->second : "";
}

RegisterAllocator::RegisterAllocator(
    const std::vector<std::shared_ptr<IrBasicBlock>>& blocks,
    const std::unordered_set<std::string>& global_names)
    : _blocks(blocks), _global_names(global_names)
{
}

std::string RegisterAllocator::make_key(const std::string& temp_name) const
{
    return _current_function + "::" + temp_name;
}

bool RegisterAllocator::needs_allocation(const IrOperand& op) const
{
    return op.type == IrOperandType::Temporary
        && !op.value.empty()
        && !_global_names.count(op.value);
}

bool RegisterAllocator::is_live_across_call(const std::string& name, uint64_t call_instr) const
{
    const auto it = _registry_life_duration.find(name);
    if (it == _registry_life_duration.end()) return false;
    return it->second.start < call_instr && call_instr < it->second.end;
}

void RegisterAllocator::reset_free_pool()
{
    _registry_life_duration.clear();
    _free_regs.clear();
    _free_regs.reserve(k_reg_free_count);
    for (int r = k_reg_free_first; r <= k_reg_free_last; ++r)
        _free_regs.push_back(r);
}

/* Lifetime duration*/

void RegisterAllocator::compute_instruction_duration(const IrInstruction& instr)
{
    auto update_duration = [&](const IrOperand& op)
    {
        if (op.type != IrOperandType::Temporary || op.value.empty()) return;

        const auto it = _registry_life_duration.find(op.value);
        if (it != _registry_life_duration.end())
        {
            it->second.start = std::min(it->second.start, instr.instr_nbr);
            it->second.end = std::max(it->second.end, instr.instr_nbr);
        }
        else
        {
            _registry_life_duration[op.value] = {instr.instr_nbr, instr.instr_nbr};
        }
    };

    update_duration(instr.arg1);
    update_duration(instr.arg2);
    update_duration(instr.result);
    for (const auto& arg : instr.call_args) update_duration(arg);
}

void RegisterAllocator::compute_function_duration(
    const IrBasicBlock& entry, uint64_t start_offset)
{
    for (const auto& instr : entry.instructions)
        compute_instruction_duration(instr);

    for (uint64_t i = start_offset + 1;
         i < _blocks.size() && !_blocks[i]->is_function_entry; ++i)
    {
        for (const auto& instr : _blocks[i]->instructions)
            compute_instruction_duration(instr);
    }
}

void RegisterAllocator::extend_lifetimes_for_back_jumps(uint64_t start_offset)
{
    // Create Label -> First instruction_nbr for this function
    std::unordered_map<std::string, uint64_t> label_to_first_instr;
    for (uint64_t i = start_offset; i < _blocks.size() &&
         (i == start_offset || !_blocks[i]->is_function_entry); ++i)
    {
        const IrBasicBlock& block = *_blocks[i];
        if (!block.instructions.empty())
            label_to_first_instr[block.label] = block.instructions.front().instr_nbr;
    }

    // for each block that terminated with a JUMP to previous block
    for (uint64_t i = start_offset; i < _blocks.size() &&
         (i == start_offset || !_blocks[i]->is_function_entry); ++i)
    {
        const IrBasicBlock& block = *_blocks[i];

        if (block.terminator != IrBlockTerminator::JUMP) continue;
        if (block.instructions.empty()) continue;
        const auto successor = block.successor.lock();
        if (!successor) continue;

        const auto target_it = label_to_first_instr.find(successor->label);

        if (target_it == label_to_first_instr.end()) continue;

        const uint64_t target_first = target_it->second;
        const uint64_t this_last = block.instructions.back().instr_nbr;

        if (target_first >= this_last) continue; // Continue logic only if it's a back jump

        for (auto& [name, life] : _registry_life_duration)
        {
            const bool is_loop_carried = _parameter_names.count(name) || life.start < target_first;
            if (is_loop_carried && life.start <= this_last && life.end >= target_first)
                life.end = std::max(life.end, this_last + 1);
        }
    }
}

/* Linear Scan register allocation */

void RegisterAllocator::assign_parameters(const IrBasicBlock& entry)
{
    _parameter_names.clear();

    const uint64_t first_instr = entry.instructions.empty() ? 0 : entry.instructions.front().instr_nbr;

    for (std::size_t i = 0; i < entry.parameters.size() && i < 6; ++i)
    {
        const std::string& param = entry.parameters[i];
        _parameter_names.insert(param);
        _result.reg_map[make_key(param)] = reg_name(k_reg_param_first + static_cast<int>(i));
        _registry_life_duration[param] = { first_instr, first_instr };
    }
}

void RegisterAllocator::linear_scan_function()
{
    std::vector<std::pair<std::string, LifeDuration>> intervals(
        _registry_life_duration.begin(), _registry_life_duration.end());

    std::ranges::sort(intervals,
                      [](const auto& a, const auto& b)
                      {
                          return a.second.start < b.second.start;
                      });

    std::multimap<uint64_t, std::string> active;

    for (const auto& [name, life] : intervals)
    {
        // Expire terminated intervals
        for (auto it = active.begin(); it != active.end() && it->first < life.start;)
        {
            const std::string expired_key = make_key(it->second);
            const std::string& reg = _result.reg_map.at(expired_key);
            if (!reg.empty())
                _free_regs.push_back(std::stoi(reg.substr(1)));
            it = active.erase(it);
        }

        const std::string key = make_key(name);
        if (_result.reg_map.count(key))
        {
            active.emplace(life.end, name);
            continue;
        }

        if (_free_regs.empty())
        {
            Linter::instance().report(
                "register pool exhausted in '" + _current_function + "'", "", 0, 0);
            continue;
        }

        const int reg = _free_regs.front();
        _free_regs.erase(_free_regs.begin());
        _result.reg_map[key] = reg_name(reg);
        active.emplace(life.end, name);
    }
}

/* Spills around call */

void RegisterAllocator::insert_spills_for_block(IrBasicBlock& block)
{
    std::vector<IrInstruction> result;
    result.reserve(block.instructions.size());

    for (const auto& instr : block.instructions)
    {
        if (instr.op != IrOpCode::CALL && instr.op != IrOpCode::BUILTIN_CALL)
        {
            result.push_back(instr);
            continue;
        }

        std::vector<std::string> to_spill;
        for (const auto& [name, life] : _registry_life_duration)
        {
            if (!is_live_across_call(name, instr.instr_nbr)) continue;
            const std::string key = make_key(name);
            const auto it = _result.reg_map.find(key);
            if (it != _result.reg_map.end() && !it->second.empty())
                to_spill.push_back(name);
        }

        auto make_spill = [&](IrOpCode op, const std::string& name) -> IrInstruction
        {
            IrInstruction s;
            s.op = op;
            s.arg1 = IrOperand{IrOperandType::Temporary, name};
            return s;
        };

        for (const auto& name : to_spill)
            result.push_back(make_spill(IrOpCode::SPILL_SAVE, name));

        result.push_back(instr);

        for (auto it = to_spill.rbegin(); it != to_spill.rend(); ++it)
            result.push_back(make_spill(IrOpCode::SPILL_RESTORE, *it));
    }

    block.instructions = std::move(result);
}

void RegisterAllocator::insert_function_spills(uint64_t start_index)
{
    for (uint64_t i = start_index; i < _blocks.size(); ++i)
    {
        if (i != start_index && _blocks[i]->is_function_entry) break;
        insert_spills_for_block(*_blocks[i]);
    }
}

RegisterAllocation RegisterAllocator::allocate()
{
    uint64_t index = 0;
    for (const auto& block_ptr : _blocks)
    {
        if (block_ptr->is_function_entry)
        {
            _current_function = block_ptr->function_name;
            reset_free_pool();

            assign_parameters(*block_ptr); // Collect function parameters
            compute_function_duration(*block_ptr, index); // Compute the basic lifetime duration for each label
            extend_lifetimes_for_back_jumps(index); // Compute back jump durations
            linear_scan_function(); // // Allocate registries
            insert_function_spills(index); // Spills
        }
        index++;
    }
    return std::move(_result);
}
