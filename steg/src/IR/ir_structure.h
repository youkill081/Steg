//
// Created by Roumite on 19/03/2026.
//

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <functional>

#include "ast/ASTTypeNode.h"

namespace compiler
{
    enum class IrOperandType
    {
        Temporary,
        Constant,
        Label,
    };

    enum class IrValueType
    {
        UNKNOWN,
        BOOL,
        UINT8, UINT16, UINT32,
        INT,
        FLOAT,
        FILE,
        CLOCK
    };

    struct IrOperand {
        IrOperandType type;
        std::string value;
        IrValueType value_type = IrValueType::UNKNOWN;
        uint8_t ptr_depth = 0;

        [[nodiscard]] int32_t as_int() const { return std::stoi(value); }
        [[nodiscard]] float as_float() const { return std::stof(value); }
        [[nodiscard]] bool is_signed() const {
            return value_type == IrValueType::INT;
        }
        [[nodiscard]] bool is_float() const {
            return value_type == IrValueType::FLOAT;
        }
        [[nodiscard]] bool is_ptr() const {
            return ptr_depth > 0;
        }

        [[nodiscard]] bool empty() const { return value.empty(); }
    };

    inline bool ir_operand_type_is_ptr(IrOperand op)
    {
        return op.ptr_depth > 0;
    }

    enum class IrOpCode {
        ADD, SUB, MUL, DIV, MOD,
        NEG, NOT,

        FADD, FSUB, FMUL, FDIV, FMOD, // Float versions
        FNEG,

        SDIV, SMUL,

        BIT_AND, BIT_OR, BIT_XOR,
        BIT_NOT, BIT_SHIFT_LEFT, BIT_SHIFT_RIGHT,

        SIGNED_BIT_SHIFT_RIGHT, // Signed version

        EQ, NEQ,
        LT, GT, LEQ, GEQ,
        SLT, SGT, SLEQ, SGEQ, // Signed version

        FEQ, FNEQ, // Float versions
        FLT, FGT, FLEQ, FGEQ,

        ITOF, UTOF, // int->float ; unsigned->float
        FTOI, FTOU, // float->int ; float->unsigned

        ZEXTEND, // Sign extention
        TRUNC, // Sign packing

        AND, OR,

        COPY, // result = args
        ADDR_OF, // result = #arg1

        LOAD_ARR, // result = arg1[arg2]
        STORE_ARR, // result[arg1] = arg2

        LABEL, GOTO,

        LOAD_8,  LOAD_16,  LOAD_32,
        STORE_8, STORE_16, STORE_32,

        SPILL_SAVE, SPILL_RESTORE,

        // High levels operator
        DEREF, // result = *arg1
        DEREF_STORE, // *result = arg1 (result is a pointer)

        CALL,
        BUILTIN_CALL,
    };

    inline uint64_t current_instr_nbr = 0;
    struct IrInstruction {
        IrOpCode op;
        IrOperand result;
        IrOperand arg1;
        IrOperand arg2;
        uint64_t instr_nbr = current_instr_nbr++;

        std::vector<IrOperand> call_args; // For function call
        std::optional<std::function<IrOperand(IrOperand, IrOperand)>> exec = std::nullopt; // Used to compute folding values

        static IrInstruction make_3addr(
            const IrOpCode op,
            const std::string &res,
            const std::string &a1,
            const std::string &a2)
        {
            return {
                op,
                {IrOperandType::Temporary, res},
                {IrOperandType::Temporary, a1},
                {IrOperandType::Temporary, a2}
            };
        }
    };

    struct IrFile {
        std::string name;
        std::string path;
        std::string absolute_path;
    };

    struct IrGlobal {
        std::string name;
        IrValueType type = IrValueType::UNKNOWN;
        ASTTypeNode::Types default_ast_type = ASTTypeNode::Types::VOID;
        IrOperand initial_value = {};
        uint8_t ptr_depth = 0;
    };

    enum class IrBlockTerminator {
        NONE,
        JUMP,
        BRANCH,
        RETURN,
        RETURN_VOID,
    };

    struct IrBasicBlock
    {
        std::string label;
        std::vector<IrInstruction> instructions;

        bool is_function_entry = false;
        std::string function_name;
        std::vector<std::string> parameters;

        IrBlockTerminator terminator = IrBlockTerminator::NONE;
        IrOperand condition_operand = {};
        IrOperand return_operand = {};

        std::weak_ptr<IrBasicBlock> successor;
        std::weak_ptr<IrBasicBlock> false_successor;
    };
}
