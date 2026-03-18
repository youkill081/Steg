//
// Created by Roumite on 18/03/2026.
//

#include "TokenMap.h"

std::string compiler::TokenMap::make_key(const LexerToken& token)
{
    return std::to_string(token.line_number) + ":" + std::to_string(token.column_number);
}

void compiler::TokenMap::add_token(const LexerToken& token)
{
    _map[make_key(token)] = token;
}

void compiler::TokenMap::set_token(const LexerToken& token, LexerTokenCategory category)
{
    auto key = make_key(token);
    auto it = _map.find(key);
    if (it != _map.end())
        it->second.category = category;
}

std::unordered_map<std::string, compiler::LexerToken>& compiler::TokenMap::get_map()
{
    return _map;
}

std::vector<compiler::LexerToken> compiler::TokenMap::ordered()
{
    std::vector<LexerToken> result;
    result.reserve(_map.size());
    for (const auto& [key, token] : _map)
        result.push_back(token);

    std::ranges::sort(result, [](const LexerToken& a, const LexerToken& b) {
        if (a.line_number != b.line_number)
            return a.line_number < b.line_number;
        return a.column_number < b.column_number;
    });
    return result;
}


