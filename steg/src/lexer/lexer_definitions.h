//
// Created by Roumite on 14/03/2026.
//

#pragma once

#include <array>
#include <string_view>
#include <unordered_map>

enum LexerTokensTypes
{
    TOKEN_KEYWORD,
    TOKEN_TYPE,
    TOKEN_IDENTIFIER,
    TOKEN_LITERAL,
    TOKEN_OPERAND,
    TOKEN_PUNCTUATION,
    TOKEN_STRING,
    TOKEN_EOF // Automatically added by lexer at the end of the file
};

struct TokenEntry {
    std::string_view name;
    LexerTokensTypes type;
};

constexpr std::array KEYWORDS = {
    // Keywords
    TokenEntry{"fn", TOKEN_KEYWORD},
    TokenEntry{"if", TOKEN_KEYWORD},
    TokenEntry{"while", TOKEN_KEYWORD},
    TokenEntry{"return", TOKEN_KEYWORD},
    TokenEntry{"for", TOKEN_KEYWORD},
    TokenEntry{"export", TOKEN_KEYWORD},

    // Types
    TokenEntry{"uint8", TOKEN_TYPE},
    TokenEntry{"int8", TOKEN_TYPE},
    TokenEntry{"uint16", TOKEN_TYPE},
    TokenEntry{"int16", TOKEN_TYPE},
    TokenEntry{"uint32", TOKEN_TYPE},
    TokenEntry{"int32", TOKEN_TYPE},
    TokenEntry{"bool", TOKEN_TYPE},
    TokenEntry{"void", TOKEN_TYPE},

    // Literals
    TokenEntry{"TRUE", TOKEN_LITERAL},
    TokenEntry{"TRUE", TOKEN_LITERAL},

    // Operands
    TokenEntry{"+", TOKEN_OPERAND},
    TokenEntry{"-", TOKEN_OPERAND},
    TokenEntry{"*", TOKEN_OPERAND},
    TokenEntry{"/", TOKEN_OPERAND},
    TokenEntry{"%", TOKEN_OPERAND},
    TokenEntry{"==", TOKEN_OPERAND},
    TokenEntry{"!=", TOKEN_OPERAND},
    TokenEntry{"<", TOKEN_OPERAND},
    TokenEntry{">", TOKEN_OPERAND},
    TokenEntry{"<=", TOKEN_OPERAND},
    TokenEntry{">=", TOKEN_OPERAND},
    TokenEntry{"&&", TOKEN_OPERAND},
    TokenEntry{"||", TOKEN_OPERAND},
    TokenEntry{"!", TOKEN_OPERAND},
    TokenEntry{"&", TOKEN_OPERAND},
    TokenEntry{"#", TOKEN_OPERAND},

    // Punctuation
    TokenEntry{"(", TOKEN_PUNCTUATION},
    TokenEntry{")", TOKEN_PUNCTUATION},
    TokenEntry{"{", TOKEN_PUNCTUATION},
    TokenEntry{"}", TOKEN_PUNCTUATION},
    TokenEntry{"[", TOKEN_PUNCTUATION},
    TokenEntry{"]", TOKEN_PUNCTUATION},
    TokenEntry{",", TOKEN_PUNCTUATION},
    TokenEntry{";", TOKEN_PUNCTUATION},
    TokenEntry{"->", TOKEN_PUNCTUATION},
};