#include "Server.h"
#include "Transport.h"

#include "../handlers/Initialize.h"
#include "../handlers/Shutdown.h"
#include "../handlers/Exit.h"
#include "../handlers/DidOpen.h"
#include "../handlers/SemanticTokensFull.h"

#include <iostream>
#include <nlohmann/json.hpp>

#include "handlers/DidChange.h"
#include "lsp/Diagnostics.h"
#include "lsp/Tokens.h"

namespace core
{

void runServer(std::istream& in, std::ostream& out)
{
    bool running = true;

    while(running)
    {
        auto body = readMessage(in);
        if(!body)
        {
            std::cerr << "[lsp] connexion fermée.\n";
            break;
        }

        nlohmann::json msg;
        try
        {
            msg = nlohmann::json::parse(*body);
        }
        catch(const nlohmann::json::parse_error& e)
        {
            std::cerr << "[lsp] JSON parse error: " << e.what() << '\n';
            continue;
        }

        if(!msg.contains("method") || !msg["method"].is_string()) continue;

        const auto        method = msg["method"].get<std::string>();
        const nlohmann::json  id     = msg.value("id",     nlohmann::json{});
        const nlohmann::json  params = msg.value("params", nlohmann::json::object());

        std::cerr << "[lsp] " << method << '\n';

        // ── Dispatch ──────────────────────────────────────────────────────────
        if(method == "initialize")
            handlers::onInitialize(id, params, out);

        else if(method == "initialized")
            {}

        else if(method == "shutdown")
            handlers::onShutdown(id, params, out);

        else if(method == "exit")
            running = handlers::onExit();

        else if(method == "textDocument/didOpen")
            handlers::onDidOpen(id, params, out);

        else if(method == "textDocument/semanticTokens/full")
            handlers::onSemanticTokensFull(id, params, out);

        else if(method == "textDocument/didChange")
            handlers::onDidChange(id, params, out);

        else if(method == "textDocument/didSave")
        {
            if(params.contains("textDocument"))
            {
                const auto uri = params["textDocument"]["uri"].get<std::string>();
                lsp::publishDiagnostics(uri, lsp::uriToPath(uri), out);
            }
        }
    }
}

} // namespace core