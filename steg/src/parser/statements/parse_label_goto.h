//
// Created by Roumite on 11/04/2026.
//

#pragma once

#include "ast/ASTStatementNode.h"
#include "parser/parser_utils.h"
#include "parser/monadic/monadic.hpp"

namespace compiler
{
    inline Parser<std::unique_ptr<ASTLabelStatement>, TokenSpan> parseLabelStatement =
        map(seq(parseToken<TOKEN_IDENTIFIER>, parseToken<TOKEN_PUNCTUATION_COLON>), [](auto data)
        {
            auto [identifier, colon] = std::move(data);
            return std::make_unique<ASTLabelStatement>(identifier, identifier.value);
        });

    inline Parser<std::unique_ptr<ASTGotoStatement>, TokenSpan> parseGotoStatement =
        map(seq(parseToken<TOKEN_KEYWORD_GOTO>, lintedParseToken<TOKEN_IDENTIFIER>), [](auto data)
        {
            auto [goto_token, identifier] = std::move(data);
            return std::make_unique<ASTGotoStatement>(identifier, identifier.value);
        });
}