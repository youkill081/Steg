//
// Created by Roumite on 18/03/2026.
//

#pragma once

#include <unordered_map>

#include "Lexer.h"

namespace compiler
{
    class TokenMap
    {
    private:
        std::unordered_map<std::string, LexerToken> _map;

        static std::string make_key(const LexerToken& token);
    public:
        void add_token(const LexerToken& token);
        void set_token(const LexerToken& token, LexerTokenCategory category);
        std::unordered_map<std::string, LexerToken> &get_map();
        std::vector<LexerToken> ordered();
    };
}
