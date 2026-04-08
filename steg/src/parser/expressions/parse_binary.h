//
// Created by Roumite on 16/03/2026.
//

#pragma once

#include "parse_expressions_utils.h"

namespace compiler
{
    /* Layer 3 */

    inline Parser<InfixPart, TokenSpan> parseMultiplicationPart = map(
        seq(parseToken<TOKEN_OPERATOR_MULTIPLY>, lint<"Expected expression after '*'">(compiler::ref(parseUnary))),
        [](auto data) { return InfixPart{ASTBinaryExpressionNode::MULTIPLICATION, std::move(std::get<1>(data)), std::get<0>(data)}; }
    );

    inline Parser<InfixPart, TokenSpan> parseDivisionPart = map(
        seq(parseToken<TOKEN_OPERATOR_DIVIDE>, lint<"Expected expression after '/'">(compiler::ref(parseUnary))),
        [](auto data) { return InfixPart{ASTBinaryExpressionNode::DIVISION, std::move(std::get<1>(data)), std::get<0>(data)}; }
    );

    inline Parser<InfixPart, TokenSpan> parseModuloPart = map(
        seq(parseToken<TOKEN_OPERATOR_MODULUS>, lint<"Expected expression after '%'">(compiler::ref(parseUnary))),
        [](auto data) { return InfixPart{ASTBinaryExpressionNode::MODULO, std::move(std::get<1>(data)), std::get<0>(data)}; }
    );

    /* Layer 2  */

    inline Parser<InfixPart, TokenSpan> parseAdditionPart = map(
        seq(parseToken<TOKEN_OPERATOR_PLUS>, lint<"Expected expression after '+'">(compiler::ref(parseLayer4))),
        [](auto data) { return InfixPart{ASTBinaryExpressionNode::ADDITION, std::move(std::get<1>(data)), std::get<0>(data)}; }
    );

    inline Parser<InfixPart, TokenSpan> parseSubtractionPart = map(
        seq(parseToken<TOKEN_OPERATOR_MINUS>, lint<"Expected expression after '-'">(compiler::ref(parseLayer4))),
        [](auto data) { return InfixPart{ASTBinaryExpressionNode::SUBTRACTION, std::move(std::get<1>(data)), std::get<0>(data)}; }
    );

    /* Shift Layer */

    inline Parser<InfixPart, TokenSpan> parseShiftLeft = map(
    seq(parseToken<TOKEN_OPERATOR_BIT_SHIFT_LEFT>, lint<"Expected expression after '<<'">(compiler::ref(parseLayer3))),
    [](auto data) { return InfixPart{ASTBinaryExpressionNode::BIT_SHIFT_LEFT, std::move(std::get<1>(data)), std::get<0>(data)}; }
);

    inline Parser<InfixPart, TokenSpan> parseShiftRight = map(
        seq(parseToken<TOKEN_OPERATOR_BIT_SHIFT_RIGHT>, lint<"Expected expression after '>>'">(compiler::ref(parseLayer3))),
        [](auto data) { return InfixPart{ASTBinaryExpressionNode::BIT_SHIFT_RIGHT, std::move(std::get<1>(data)), std::get<0>(data)}; }
    );

    /* Bitwise parsers */

    inline Parser<InfixPart, TokenSpan> parseBitAnd = map(
    seq(parseToken<TOKEN_OPERATOR_BIT_AND>, lint<"Expected expression after '&'">(compiler::ref(parseLayer2))),
    [](auto data) { return InfixPart{ASTBinaryExpressionNode::BIT_AND, std::move(std::get<1>(data)), std::get<0>(data)}; }
);

    inline Parser<InfixPart, TokenSpan> parseBitXor = map(
        seq(parseToken<TOKEN_OPERATOR_BIT_XOR>, lint<"Expected expression after '^'">(compiler::ref(parseBitAndLayer))),
        [](auto data) { return InfixPart{ASTBinaryExpressionNode::BIT_XOR, std::move(std::get<1>(data)), std::get<0>(data)}; }
    );

    inline Parser<InfixPart, TokenSpan> parseBitOr = map(
        seq(parseToken<TOKEN_OPERATOR_BIT_OR>, lint<"Expected expression after '|'">(compiler::ref(parseBitXorLayer))),
        [](auto data) { return InfixPart{ASTBinaryExpressionNode::BIT_OR, std::move(std::get<1>(data)), std::get<0>(data)}; }
    );

    /* Layer 1 */

    inline Parser<InfixPart, TokenSpan> parseComparisonEqual = map(
        seq(parseToken<TOKEN_COMPARISON_EQUAL>, lint<"Expected expression after '=='">(compiler::ref(parseShiftLayer))),
        [](auto data) { return InfixPart{ASTBinaryExpressionNode::COMPARISON_EQUAL, std::move(std::get<1>(data)), std::get<0>(data)}; }
    );

    inline Parser<InfixPart, TokenSpan> parseComparisonNotEqual = map(
        seq(parseToken<TOKEN_COMPARISON_NOT_EQUAL>, lint<"Expected expression after '!='">(compiler::ref(parseShiftLayer))),
        [](auto data) { return InfixPart{ASTBinaryExpressionNode::COMPARISON_NOT_EQUAL, std::move(std::get<1>(data)), std::get<0>(data)}; }
    );

    inline Parser<InfixPart, TokenSpan> parseComparisonLess = map(
        seq(parseToken<TOKEN_COMPARISON_LESS>, lint<"Expected expression after '<'">(compiler::ref(parseShiftLayer))),
        [](auto data) { return InfixPart{ASTBinaryExpressionNode::COMPARISON_LESS, std::move(std::get<1>(data)), std::get<0>(data)}; }
    );

    inline Parser<InfixPart, TokenSpan> parseComparisonGreater = map(
        seq(parseToken<TOKEN_COMPARISON_GREATER>, lint<"Expected expression after '>'">(compiler::ref(parseShiftLayer))),
        [](auto data) { return InfixPart{ASTBinaryExpressionNode::COMPARISON_GREATER, std::move(std::get<1>(data)), std::get<0>(data)}; }
    );

    inline Parser<InfixPart, TokenSpan> parseComparisonLessOrEqual = map(
        seq(parseToken<TOKEN_COMPARISON_LESS_OR_EQUAL>, lint<"Expected expression after '<='">(compiler::ref(parseShiftLayer))),
        [](auto data) { return InfixPart{ASTBinaryExpressionNode::COMPARISON_LESS_OR_EQUAL, std::move(std::get<1>(data)), std::get<0>(data)}; }
    );

    inline Parser<InfixPart, TokenSpan> parseComparisonGreaterOrEqual = map(
        seq(parseToken<TOKEN_COMPARISON_GREATER_OR_EQUAL>, lint<"Expected expression after '>='">(compiler::ref(parseShiftLayer))),
        [](auto data) { return InfixPart{ASTBinaryExpressionNode::COMPARISON_GREATER_OR_EQUAL, std::move(std::get<1>(data)), std::get<0>(data)}; }
    );

    /* Layer 0 */

    inline Parser<InfixPart, TokenSpan> parseComparisonAnd = map(
        seq(parseToken<TOKEN_COMPARISON_AND>, lint<"Expected expression after '&&'">(compiler::ref(parseBitOrLayer))),
        [](auto data) { return InfixPart{ASTBinaryExpressionNode::COMPARISON_AND, std::move(std::get<1>(data)), std::get<0>(data)}; }
    );

    inline Parser<InfixPart, TokenSpan> parseComparisonOr = map(
        seq(parseToken<TOKEN_COMPARISON_OR>, lint<"Expected expression after '||'">(compiler::ref(parseBitOrLayer))),
        [](auto data) { return InfixPart{ASTBinaryExpressionNode::COMPARISON_OR, std::move(std::get<1>(data)), std::get<0>(data)}; }
    );
}