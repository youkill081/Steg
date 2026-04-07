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
    _call_sites.clear();

    _free_volatile.clear();
    for (int r = k_reg_volatile_first; r <= k_reg_volatile_last; ++r)
        _free_volatile.push_back(r);

    _free_preserved.clear();
    for (int r = k_reg_preserved_first; r <= k_reg_preserved_last; ++r)
        _free_preserved.push_back(r);
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
    _call_sites.clear();

    auto scan_block = [&](const IrBasicBlock& block)
    {
        for (const auto& instr : block.instructions)
        {
            compute_instruction_duration(instr);
            if (instr.op == IrOpCode::CALL || instr.op == IrOpCode::BUILTIN_CALL)
                _call_sites.insert(instr.instr_nbr);
        }
    };

    scan_block(entry);
    for (uint64_t i = start_offset + 1;
         i < _blocks.size() && !_blocks[i]->is_function_entry; ++i)
        scan_block(*_blocks[i]);
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

bool RegisterAllocator::spans_call(const LifeDuration &life) const
{
    for (const uint64_t call : _call_sites)
    {
        if (life.start < call && call < life.end)
            return true;
    }
    return false;
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

    std::multimap<uint64_t, std::pair<std::string, bool>> active;

    for (const auto& [name, life] : intervals)
    {
        for (auto it = active.begin();
             it != active.end() && it->first < life.start;)
        {
            const auto& [expired_name, was_preserved] = it->second;
            const std::string& reg = _result.reg_map.at(make_key(expired_name));
            const int reg_idx = std::stoi(reg.substr(1));

            const bool is_param = (reg_idx >= k_reg_param_first && reg_idx <= k_reg_param_last);
            if (!is_param)
            {
                if (was_preserved) _free_preserved.push_back(reg_idx);
                else _free_volatile.push_back(reg_idx);
            }

            it = active.erase(it);
        }

        const std::string key = make_key(name);
        if (_result.reg_map.count(key))
        {
            active.emplace(life.end, std::make_pair(name, false));
            continue;
        }

        const bool needs_preserved = spans_call(life);

        auto try_alloc = [&](std::vector<int>& pool, bool is_preserved) -> bool
        {
            if (pool.empty())
                return false;
            const int reg = pool.front();
            pool.erase(pool.begin());
            _result.reg_map[key] = reg_name(reg);
            active.emplace(life.end, std::make_pair(name, is_preserved));
            return true;
        };

        if (needs_preserved)
        {
            if (!try_alloc(_free_preserved, true) && !try_alloc(_free_volatile, false))
                Linter::instance().report("register pool exhausted in '" + _current_function + "'", "", 0, 0);
        }
        else
        {
            if (!try_alloc(_free_volatile, false) && !try_alloc(_free_preserved, true))
                Linter::instance().report("register pool exhausted in '" + _current_function + "'", "", 0, 0);
        }
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
            if (!is_live_across_call(name, instr.instr_nbr))
                continue;

            const std::string key = make_key(name);
            const auto it = _result.reg_map.find(key);
            if (it == _result.reg_map.end() || it->second.empty())
                continue;

            const int reg_idx = std::stoi(it->second.substr(1));
            if (reg_idx >= k_reg_volatile_first && reg_idx <= k_reg_volatile_last)
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

void RegisterAllocator::insert_preserved_saves(uint64_t start_index)
{
    std::vector<int> used_preserved;
    for (const auto& [key, reg] : _result.reg_map)
    {
        if (!key.starts_with(_current_function + "::"))
            continue;
        if (reg.empty())
            continue;
        const int idx = std::stoi(reg.substr(1));
        if (idx >= k_reg_preserved_first && idx <= k_reg_preserved_last)
            used_preserved.push_back(idx);
    }

    std::ranges::sort(used_preserved);
    used_preserved.erase(std::ranges::unique(used_preserved).begin(),
                         used_preserved.end());

    if (used_preserved.empty())
        return;

    // CORRECTION : On capture par référence [&] pour accéder à _result et make_key
    auto make_spill = [&](IrOpCode op, int reg_idx) -> IrInstruction
    {
        std::string rname = reg_name(reg_idx);

        // CORRECTION : On force l'enregistrement du registre physique dans la map
        // Ainsi, quand l'ASM generator fera lookup("...", "R21"), il recevra bien "R21" !
        _result.reg_map[make_key(rname)] = rname;

        IrInstruction s;
        s.op   = op;
        s.arg1 = IrOperand{IrOperandType::Temporary, rname};
        return s;
    };

    // Prologue : insérer en bloc pour préserver l'ordre
    IrBasicBlock& entry = *_blocks[start_index];
    std::vector<IrInstruction> prologue;
    for (int r : used_preserved)
        prologue.push_back(make_spill(IrOpCode::SPILL_SAVE, r));
    entry.instructions.insert(entry.instructions.begin(),
                               prologue.begin(), prologue.end());

    // Épilogue : SPILL_RESTORE en ordre inverse avant chaque RETURN
    for (uint64_t i = start_index; i < _blocks.size(); ++i)
    {
        if (i != start_index && _blocks[i]->is_function_entry)
            break;
        IrBasicBlock& block = *_blocks[i];

        const bool is_return = block.terminator == IrBlockTerminator::RETURN
                            || block.terminator == IrBlockTerminator::RETURN_VOID;
        if (!is_return)
            continue;

        for (auto it = used_preserved.rbegin(); it != used_preserved.rend(); ++it)
            block.instructions.push_back(make_spill(IrOpCode::SPILL_RESTORE, *it));
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
            insert_preserved_saves(index); // Spills reserved registries if needed
        }
        index++;
    }
    return std::move(_result);
}
