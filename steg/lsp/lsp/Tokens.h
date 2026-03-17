#pragma once

#include <nlohmann/json.hpp>
#include <string>

namespace lsp
{
    nlohmann::json buildTokenTypesLegend();
    std::string uriToPath(const std::string& uri);
    nlohmann::json encodeTokens(const std::string& uri, const std::string& filePath);
} // namespace lsp