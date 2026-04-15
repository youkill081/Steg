#pragma once
#include <nlohmann/json.hpp>
#include <iosfwd>
#include <string>

namespace lsp
{
    void publishDiagnostics(
        const std::string& uri,
        const std::string& filePath,
        std::ostream& out
    );
}