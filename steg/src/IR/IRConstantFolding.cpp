//
// Created by Roumite on 06/04/2026.
//

#include "IRConstantFolding.h"
#include <bit>

#include "utils/exec.h"

using namespace compiler;

const std::unordered_map<IrOpCode, ExecFn> IRConstantFolding::_binary_exec = {
    {IrOpCode::ADD, exec_add}, {IrOpCode::SUB, exec_sub},
    {IrOpCode::MUL, exec_mul}, {IrOpCode::DIV, exec_div},
    {IrOpCode::MOD, exec_mod},

    {IrOpCode::BIT_AND, exec_bit_and}, {IrOpCode::BIT_OR, exec_bit_or}, {IrOpCode::BIT_XOR, exec_bit_xor},
    {IrOpCode::BIT_SHIFT_LEFT, exec_bit_shift_left}, {IrOpCode::BIT_SHIFT_RIGHT, exec_bit_shift_right},
    {IrOpCode::SIGNED_BIT_SHIFT_RIGHT, exec_bit_signed_shift_right},

    {IrOpCode::FADD, exec_fadd}, {IrOpCode::FSUB, exec_fsub},
    {IrOpCode::FMUL, exec_fmul}, {IrOpCode::FDIV, exec_fdiv},
    {IrOpCode::FMOD, exec_fmod},

    {IrOpCode::SDIV, exec_sdiv}, {IrOpCode::SMUL, exec_smul},

    {IrOpCode::EQ, exec_eq}, {IrOpCode::NEQ, exec_neq},
    {IrOpCode::LT, exec_lt}, {IrOpCode::GT, exec_gt},
    {IrOpCode::LEQ, exec_leq}, {IrOpCode::GEQ, exec_geq},
    {IrOpCode::SLT, exec_slt}, {IrOpCode::SGT, exec_sgt},
    {IrOpCode::SLEQ, exec_sleq}, {IrOpCode::SGEQ, exec_sgeq},
    {IrOpCode::FEQ, exec_feq}, {IrOpCode::FNEQ, exec_fneq},
    {IrOpCode::FLT, exec_flt}, {IrOpCode::FGT, exec_fgt},
    {IrOpCode::FLEQ, exec_fleq}, {IrOpCode::FGEQ, exec_fgeq},

    {IrOpCode::AND, exec_and}, {IrOpCode::OR, exec_or},
};

const std::unordered_map<IrOpCode, ExecFn> IRConstantFolding::_unary_exec = {
    {IrOpCode::NOT, exec_not}, {IrOpCode::BIT_NOT, exec_bit_not},
    {IrOpCode::ITOF, exec_itof}, {IrOpCode::UTOF, exec_utof},
    {IrOpCode::FTOI, exec_ftoi}, {IrOpCode::FTOU, exec_ftou},
};

IRConstantFolding::IRConstantFolding(
    const std::vector<std::shared_ptr<IrBasicBlock>>& src_blocks,
    const std::vector<IrGlobal>& src_globals)
    : _src_blocks(src_blocks), _src_globals(src_globals)
{
}

void IRConstantFolding::fold()
{
    for (const auto& block_ptr : _src_blocks)
    {
        if (block_ptr->is_function_entry)
        {
            _folded_values.clear();
            _current_function_name = block_ptr->function_name;
        }
        _folded_values.clear();
        fold_block(*block_ptr);
    }
}

void IRConstantFolding::fold_block(IrBasicBlock& block)
{
    for (auto& instr : block.instructions)
        fold_instruction(instr);

    eliminate_dead_copies(block);
}

void IRConstantFolding::fold_instruction(IrInstruction& instr)
{
    propagate_operand(instr.arg1);
    propagate_operand(instr.arg2);

    if (instr.op == IrOpCode::COPY)
        propagate_operand(instr.arg1);

    // Is a foldable binary operand
    if (auto it = _binary_exec.find(instr.op); it != _binary_exec.end())
    {
        auto a = _folded_values.find(instr.arg1.value);
        auto b = _folded_values.find(instr.arg2.value);

        if (instr.arg1.type == IrOperandType::Constant)
            a = _folded_values.end();
        if (instr.arg2.type == IrOperandType::Constant)
            b = _folded_values.end();

        const bool a_known = instr.arg1.type == IrOperandType::Constant || a != _folded_values.end();
        const bool b_known = instr.arg2.type == IrOperandType::Constant || b != _folded_values.end();

        if (a_known && b_known)
        {
            auto raw_a = try_convert(instr.arg1.value, instr.arg1.value_type);
            auto raw_b = try_convert(instr.arg2.value, instr.arg2.value_type);

            if (!raw_a && a != _folded_values.end())
                raw_a = a->second;
            if (!raw_b && b != _folded_values.end())
                raw_b = b->second;

            if (raw_a && raw_b)
            {
                const uint32_t result = it->second(*raw_a, *raw_b);
                _folded_values[instr.result.value] = result;

                instr.op = IrOpCode::COPY;
                instr.arg1 = {
                    IrOperandType::Constant,
                    format_constant(result, instr.result.value_type),
                    instr.result.value_type
                };
                instr.arg2 = {};
                return;
            }
        }
    }

    // Is a foldable unary operation
    if (auto it = _unary_exec.find(instr.op); it != _unary_exec.end())
    {
        auto a = _folded_values.find(instr.arg1.value);
        const bool a_known = instr.arg1.type == IrOperandType::Constant || a != _folded_values.end();

        if (a_known)
        {
            auto raw_a = try_convert(instr.arg1.value, instr.arg1.value_type);
            if (!raw_a && a != _folded_values.end()) raw_a = a->second;

            if (raw_a)
            {
                const uint32_t result = it->second(*raw_a, 0);
                _folded_values[instr.result.value] = result;

                instr.op = IrOpCode::COPY;
                instr.arg1 = {
                    IrOperandType::Constant,
                    format_constant(result, instr.result.value_type),
                    instr.result.value_type
                };
                instr.arg2 = {};
                return;
            }
        }
    }

    update_folded(instr);
}

void IRConstantFolding::propagate_operand(IrOperand& operand) const
{
    if (operand.type != IrOperandType::Temporary)
        return;

    auto it = _folded_values.find(operand.value);
    if (it == _folded_values.end())
        return;

    operand.type = IrOperandType::Constant;
    operand.value = format_constant(it->second, operand.value_type);
}

void IRConstantFolding::update_folded(const IrInstruction& instr)
{
    if (instr.result.empty())
        return;

    if (instr.op == IrOpCode::COPY && instr.arg1.type == IrOperandType::Constant)
    {
        auto val = try_convert(instr.arg1.value, instr.result.value_type);
        if (val)
            _folded_values[instr.result.value] = *val;
        else
            _folded_values.erase(instr.result.value);
    }
    else
    {
        _folded_values.erase(instr.result.value);
    }
}


std::unordered_set<std::string> IRConstantFolding::collect_reads(const IrBasicBlock& block)
{
    std::unordered_set<std::string> reads;
    for (const auto& instr : block.instructions)
    {
        if (instr.arg1.type == IrOperandType::Temporary) reads.insert(instr.arg1.value);
        if (instr.arg2.type == IrOperandType::Temporary) reads.insert(instr.arg2.value);
        for (const auto& arg : instr.call_args)
            if (arg.type == IrOperandType::Temporary) reads.insert(arg.value);

        if (!instr.result.empty() && instr.result.value_type == IrValueType::BOOL)
            reads.insert(instr.result.value);
    }

    if (block.terminator == IrBlockTerminator::RETURN && block.return_operand.type == IrOperandType::Temporary)
        reads.insert(block.return_operand.value);

    if (block.terminator == IrBlockTerminator::BRANCH && block.condition_operand.type == IrOperandType::Temporary)
        reads.insert(block.condition_operand.value);

    return reads;
}

void IRConstantFolding::eliminate_dead_copies(IrBasicBlock& block)
{
    const auto reads = collect_reads(block);

    auto is_safe_to_eliminate = [&](const IrInstruction& instr) -> bool
    {
        if (instr.op != IrOpCode::COPY)
            return false;
        if (instr.arg1.type != IrOperandType::Constant)
            return false;

        if (reads.contains(instr.result.value))
            return false;

        const std::string& name = instr.result.value;
        if (name.empty() || name[0] != 't') // Not SSA, dangerous to eliminate
            return false;
        return std::all_of(name.begin() + 1, name.end(), ::isdigit);
    };

    std::erase_if(block.instructions, is_safe_to_eliminate);
}

std::optional<uint32_t> IRConstantFolding::try_convert(
    const std::string& value, IrValueType type)
{
    try
    {
        switch (type)
        {
        case IrValueType::INT:
            {
                auto s = static_cast<int32_t>(std::stoll(value));
                return std::bit_cast<uint32_t>(s);
            }
        case IrValueType::FLOAT:
            {
                float f = std::stof(value);
                return std::bit_cast<uint32_t>(f);
            }
        case IrValueType::BOOL:
        case IrValueType::UINT8:
        case IrValueType::UINT16:
        case IrValueType::UINT32:
            return static_cast<uint32_t>(std::stoull(value));
        default:
            return std::nullopt;
        }
    }
    catch (...) { return std::nullopt; }
}

std::string IRConstantFolding::format_constant(uint32_t raw, IrValueType type)
{
    switch (type)
    {
    case IrValueType::INT: return std::to_string(std::bit_cast<int32_t>(raw));
    case IrValueType::FLOAT: return std::format("{:.10f}", std::bit_cast<float>(raw)); // TODO : handle parsing of scientific notation in stegasm parser to prevent presision lose
    default: return std::to_string(raw);
    }
}
