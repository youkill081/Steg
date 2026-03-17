#include "DidChange.h"
#include "../lsp/DocumentStore.h"
#include "../lsp/Diagnostics.h"
#include "../lsp/Tokens.h"
#include <iostream>

namespace handlers
{
    void onDidChange(const nlohmann::json &, const nlohmann::json &params, std::ostream &out)
    {
        if(!params.contains("textDocument") || !params.contains("contentChanges")) return;

        const auto uri     = params["textDocument"]["uri"].get<std::string>();
        const auto changes = params["contentChanges"];

        if(!changes.empty() && changes[0].contains("text"))
        {
            lsp::DocumentStore::instance().set(uri, changes[0]["text"].get<std::string>());
            std::cerr << "[lsp] didChange: " << uri << '\n';

            lsp::publishDiagnostics(uri, lsp::uriToPath(uri), out);
        }
    }
}