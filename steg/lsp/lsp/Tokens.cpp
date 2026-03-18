#include "Tokens.h"

#include <cstdint>
#include <iostream>
#include <optional>

#include "DocumentStore.h"
#include "compiler.h"

namespace lsp
{

static std::optional<uint32_t> categoryToIndex(compiler::LexerTokenCategory cat)
{
    switch(cat)
    {
        case compiler::TOKEN_CATH_KEYWORD: return 0;
        case compiler::TOKEN_CATH_TYPE: return 1;
        case compiler::TOKEN_CATH_OPERATOR: return 2;
        case compiler::TOKEN_CATH_NUMBER: return 3;
        case compiler::TOKEN_CATH_STRING: return 4;
        case compiler::TOKEN_CATH_CONSTANT: return 5;
        case compiler::TOKEN_CATH_FUNCTION: return 6;
        case compiler::TOKEN_CATH_FUNCTION_CALL: return 7;
        case compiler::TOKEN_CATH_PARAMETER: return 8;
        case compiler::TOKEN_CATH_VARIABLE: return 9;
        default:                            return std::nullopt;
    }
}

nlohmann::json buildTokenTypesLegend()
{
    return nlohmann::json::array({
        "keyword", "type", "operator", "number", "string", "variable",
        "function", "function", "parameter", "variable"
    });
}

nlohmann::json buildTokenModifiersLegend()
{
    return nlohmann::json::array({
        "declaration",
        "readonly",
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
        compiler::Linter::instance().clear();

        auto stored = lsp::DocumentStore::instance().get(uri);

        compiler::TextParser parser = stored.has_value()
            ? compiler::TextParser::from_string(*stored, filePath)
            : compiler::TextParser::from_file(filePath);

        auto result = compiler::analyze(parser);

        if (!result.has_value())
            return data;

        uint32_t prevLine  = 0;
        uint32_t prevStart = 0;

        for(const auto& token : result->tokens.ordered())
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

            uint32_t modifier = 0;
            if (token.category == compiler::TOKEN_CATH_FUNCTION)
                modifier = 1;
            data.push_back(modifier);


            prevLine  = line;
            prevStart = start;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "[lsp] tokens error '" << filePath << "': " << e.what() << '\n';
    }

    return data;
}
}