#include "Tokens.h"

#include <cstdint>
#include <iostream>
#include <optional>

#include "DocumentStore.h"
#include "lexer/Lexer.h"
#include "lexer/lexer_definitions.h"
#include "lexer/TextParser.h"

namespace lsp
{

// ── Mapping catégorie → index de la légende ───────────────────────────────

static std::optional<uint32_t> categoryToIndex(compiler::LexerTokenCategory cat)
{
    switch(cat)
    {
        case compiler::TOKEN_CATH_KEYWORD:  return 0;
        case compiler::TOKEN_CATH_TYPE:     return 1;
        case compiler::TOKEN_CATH_OPERATOR: return 2;
        case compiler::TOKEN_CATH_NUMBER:   return 3;
        case compiler::TOKEN_CATH_STRING:   return 4;
        case compiler::TOKEN_CATH_CONSTANT: return 5;
        default:                            return std::nullopt;
    }
}

// ─────────────────────────────────────────────────────────────────────────────

nlohmann::json buildTokenTypesLegend()
{
    return nlohmann::json::array({
        "keyword", "type", "operator", "number", "string", "variable"
    });
}

std::string uriToPath(const std::string& uri)
{
    constexpr std::string_view prefix = "file:///";
    if(std::string_view(uri).starts_with(prefix))
    {
        std::string path = uri.substr(prefix.size());
#ifndef _WIN32
        return "/" + path;
#else
        return path;
#endif
    }
    return uri;
}

nlohmann::json encodeTokens(const std::string& uri, const std::string& filePath)
{
    auto data = nlohmann::json::array();
    try
    {
        auto stored = lsp::DocumentStore::instance().get(uri);

        compiler::TextParser parser = stored.has_value()
            ? compiler::TextParser::from_string(*stored, filePath)
            : compiler::TextParser::from_file(filePath);

        compiler::Lexer lexer(parser);
        lexer.compute();
        auto tokens = lexer.tokens();

        uint32_t prevLine  = 0;
        uint32_t prevStart = 0;

        for(const auto& token : tokens)
        {
            auto idx = categoryToIndex(token.category);
            if(!idx.has_value()) continue;

            const uint32_t line   = static_cast<uint32_t>(token.line_number   - 1);
            const uint32_t start  = static_cast<uint32_t>(token.column_number - 1);
            const uint32_t length = static_cast<uint32_t>(token.value.size());

            const uint32_t dLine  = line - prevLine;
            const uint32_t dStart = (dLine == 0) ? (start - prevStart) : start;

            data.push_back(dLine);
            data.push_back(dStart);
            data.push_back(length);
            data.push_back(*idx);
            data.push_back(0u);

            prevLine  = line;
            prevStart = start;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "[lsp] lexer error '" << filePath << "': " << e.what() << '\n';
    }

    return data;
}

}