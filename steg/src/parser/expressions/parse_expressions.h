//
// Created by Roumite on 16/03/2026.
//

#pragma once

#include "parser_assignments.h"
#include "parse_expressions_utils.h"
#include "parse_primary.h"
#include "parse_binary.h"
#include "parse_unary.h"

namespace compiler
{
    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parsePrimary =
        choice(
            parseParenthesizedExpr,
            parseLiteral,
            parseLiteralFloat,
            parseBool,
            parseStringLiteral ,
            parseFunctionCall ,
            parseIndexExpression ,
            parseIdentifier
        );

    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseUnary =
        choice(
            parseNegationUnary,
            parseNotUnary,
            parseBitNotUnary,
            parseAddressOfUnary,
            parseDereferenceUnary,
            parsePrimary
        );

    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseLayer4 = map(
        seq(compiler::ref(parseUnary), many(choice(parseMultiplicationPart, parseDivisionPart, parseModuloPart))),
        foldInfix
    );

    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseLayer3 = map(
        seq(compiler::ref(parseLayer4), many(choice(parseAdditionPart, parseSubtractionPart))),
        foldInfix
    );

    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseShiftLayer = map(
        seq(compiler::ref(parseLayer3), many(choice(parseShiftLeft, parseShiftRight))),
        foldInfix
    );

    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseLayer2 = map(
        seq(compiler::ref(parseShiftLayer), many(choice(
            parseComparisonEqual,
            parseComparisonNotEqual,
            parseComparisonLess,
            parseComparisonGreater,
            parseComparisonLessOrEqual,
            parseComparisonGreaterOrEqual
        ))),
        foldInfix
    );

    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseBitAndLayer = map(
        seq(compiler::ref(parseLayer2), many(parseBitAnd)),
        foldInfix
    );

    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseBitXorLayer = map(
        seq(compiler::ref(parseBitAndLayer), many(parseBitXor)),
        foldInfix
    );

    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseBitOrLayer = map(
        seq(compiler::ref(parseBitXorLayer), many(parseBitOr)),
        foldInfix
    );

    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseLayer1 = map(
        seq(compiler::ref(parseBitOrLayer), many(choice(parseComparisonAnd, parseComparisonOr))),
        foldInfix
    );

    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseLayer0 = map(
        seq(compiler::ref(parseLayer1), optional(choice(parseAssignPart, parseAssignAddPart, parseAssignSubPart, parseAssignMulPart, parseAssignDivPart))),
        foldAssign
    );

    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseExpression = parseLayer0;
}
