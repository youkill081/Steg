//
// Created by Roumite on 15/03/2026.
//

#pragma once

#include <optional>

#include "lexer/Lexer.h"
#include "lexer/lexer_definitions.h"
#include "monadic/monadic.hpp"

namespace compiler
{
    template <LexerTokensTypes token_type>
    constexpr auto parseToken = [](TokenSpan tokens) -> std::optional<Result<LexerToken, TokenSpan>>
    {
        if (tokens.empty()) return std::nullopt;
        if (tokens.front().type != token_type) return std::nullopt;
        return std::optional{Result{tokens.front(), tokens.subspan(1)}};
    };

    template <LexerTokensTypes token_type>
    constexpr auto lintedParseToken = [](TokenSpan tokens) -> std::optional<Result<LexerToken, TokenSpan>>
    {
        if (tokens.empty())
        {
            report_error(
                std::string("Expected ") + std::string(token_type_to_string.at(token_type)) + " (got EOF)",
                tokens.front()
            );
            return std::nullopt;
        }

        if (tokens.front().type != token_type)
        {
            report_error(
                std::string("Expected ") + std::string(token_type_to_string.at(token_type)) +
                " got " + std::string(token_type_to_string.at(tokens.front().type)),
                tokens.front()
            );
            return std::nullopt;
        }

        return std::optional{Result{tokens.front(), tokens.subspan(1)}};
    };
}
