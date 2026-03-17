#include "SemanticTokensFull.h"
#include "../core/Transport.h"
#include "../lsp/Tokens.h"

#include <iostream>

namespace handlers
{
    void onSemanticTokensFull(const nlohmann::json& id, const nlohmann::json& params, std::ostream& out)
    {
        if(!params.contains("textDocument") || !params["textDocument"].contains("uri"))
        {
            core::sendError(out, id, -32602, "textDocument.uri manquant");
            return;
        }

        const auto uri      = params["textDocument"]["uri"].get<std::string>();
        const auto filePath = lsp::uriToPath(uri);

        std::cerr << "[lsp] tokenizing: " << filePath << '\n';

        const nlohmann::json result = {
            {"data", lsp::encodeTokens(uri, filePath)},
        };

        core::sendResponse(out, id, result);
    }
}