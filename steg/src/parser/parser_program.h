//
// Created by Roumite on 15/03/2026.
//

#pragma once

#include "parser_utils.h"

#include <memory>

#include "ast/ASTProgramNode.h"

#include "parser_expressions.h"

namespace compilator
{
    /* Function parameters */
    inline Parser<std::unique_ptr<ASTParameterProgramNode>, TokenSpan>
    parseParameter =
         map(
            seq(
                parseTypeNoVoid,
                parseToken<TOKEN_IDENTIFIER>
            ),
            [](auto data)
            {
                return std::make_unique<ASTParameterProgramNode>(
                    std::get<1>(data).value,
                    std::move(std::get<0>(data))
                );
            }
        );

    inline Parser<std::unique_ptr<ASTParameterProgramNode>, TokenSpan>
    parseParamWithComma = parseToken<TOKEN_PUNCTUATION_COMMA> >> parseParameter;

    inline Parser<std::vector<std::unique_ptr<ASTParameterProgramNode>>, TokenSpan>
        parseFunctionParameters = parseToken<TOKEN_PUNCTUATION_LEFT_PARENTHESIS> >> map(seq(
            optional(parseParameter), many(parseParamWithComma)
        ), [](auto data)
        {
            auto params_vec = std::move(std::get<1>(data));
            auto last_param = std::move(std::get<0>(data));

            std::vector<std::unique_ptr<ASTParameterProgramNode>> parameters;

            for (auto &parameter : params_vec)
            {
                parameters.push_back(std::move(parameter));
            }
            if (last_param.has_value())
            {
                parameters.insert(parameters.begin(), std::move(*last_param)); // It's the first parameter
            }

            return parameters;
        }
    ) << parseToken<TOKEN_PUNCTUATION_RIGHT_PARENTHESIS>;

    /* Functions */

    inline Parser<LexerToken, TokenSpan> parseFunctionDeclaration = parseToken<TOKEN_KEYWORD_FUNCTION> >> parseToken<TOKEN_IDENTIFIER>;
    inline Parser<std::unique_ptr<ASTTypeNode>, TokenSpan> parseFunctionReturnType = parseToken<TOKEN_PUNCTUATION_ARROW> >> parseType;

    inline Parser<std::unique_ptr<ASTFunctionProgramNode>, TokenSpan> parseFunction =
        map(seq(
                parseFunctionDeclaration,
                parseFunctionParameters,
                parseFunctionReturnType
            ), [](auto data)
            {
                return std::make_unique<ASTFunctionProgramNode>(
                    std::move(std::get<0>(data).value),
                    std::move(std::get<1>(data)),
                    std::move(std::get<2>(data)),
                    std::make_unique<ASTBlockStatementNode>(std::vector<std::unique_ptr<ASTStatementNode>>{}),
                    true
                );
            });
}
