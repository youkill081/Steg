//
// Created by Roumite on 15/03/2026.
//

#pragma once

#include <optional>

#include "lexer/Lexer.h"
#include "lexer/lexer_definitions.h"
#include "monadic/monadic.hpp"

namespace compilator
{
    struct LexerToken;
    using TokenSpan = std::span<const LexerToken>;

    template <LexerTokensTypes token_type>
    constexpr auto parseToken = [](const TokenSpan tokens) -> std::optional<Result<LexerToken, TokenSpan>>
    {
        if (tokens.empty()) return std::nullopt;
        if (tokens.front().type != token_type) return std::nullopt;
        return Result{ tokens.front(), tokens.subspan(1) };
    };
}
