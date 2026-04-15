#include "DidOpen.h"
#include "../lsp/DocumentStore.h"
#include "../lsp/Diagnostics.h"
#include "../lsp/Tokens.h"
#include <iostream>

namespace handlers
{
    void onDidOpen(const nlohmann::json &, const nlohmann::json& params, std::ostream& out)
    {
        if(!params.contains("textDocument")) return;

        const auto& td  = params["textDocument"];
        const auto  uri = td["uri"].get<std::string>();

        if(td.contains("text"))
            lsp::DocumentStore::instance().set(uri, td["text"].get<std::string>());

        std::cerr << "[lsp] fichier ouvert: " << uri << '\n';

        lsp::publishDiagnostics(uri, lsp::uriToPath(uri), out);
    }
}