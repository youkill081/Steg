//
// Created by Roumite on 16/03/2026.
//

#pragma once

#include "parse_expressions_utils.h"
#include "parse_primary.h"
#include "parse_binary.h"

namespace compiler
{
    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parsePrimary =
        parseParenthesizedExpr | parseLiteral | parseFunctionCall | parseIdentifier;

    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseLayer3 = map(
        seq(compiler::ref(parsePrimary), many(parseMultiplicationPart | parseDivisionPart | parseModuloPart)),
        foldInfix
    );

    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseLayer2 = map(
        seq(compiler::ref(parseLayer3), many(parseAdditionPart | parseSubtractionPart)),
        foldInfix
    );

    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseLayer1 = map(
        seq(compiler::ref(parseLayer2), many(
            parseComparisonEqual |
            parseComparisonNotEqual |
            parseComparisonLess |
            parseComparisonGreater |
            parseComparisonLessOrEqual |
            parseComparisonGreaterOrEqual
        )),
        foldInfix
    );

    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseLayer0 = map(
        seq(compiler::ref(parseLayer1), many(parseComparisonAnd | parseComparisonOr)),
        foldInfix
    );

    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseExpression = parseLayer0;
}