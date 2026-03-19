#include "Diagnostics.h"
#include "DocumentStore.h"
#include "Tokens.h"
#include "../core/Transport.h"
#include "compiler.h"

#include <iostream>

namespace lsp
{

static int severityToLsp(compiler::LintData::Severity sev)
{
    switch(sev)
    {
        case compiler::LintData::Severity::ERR:  return 1;
        case compiler::LintData::Severity::HINT: return 3;
        default: return 1;
    }
}

void publishDiagnostics(const std::string& uri, const std::string& filePath, std::ostream& out)
{
    compiler::Linter::instance().clear();

    try
    {
        auto stored = DocumentStore::instance().get(uri);

        compiler::TextParser parser = stored.has_value()
            ? compiler::TextParser::from_string(*stored, filePath)
            : compiler::TextParser::from_file(filePath);

        compiler::analyze(parser);
    }
    catch(const std::exception& e)
    {
        std::cerr << "[lsp] diagnostics error: " << e.what() << '\n';
    }

    auto diagnostics = nlohmann::json::array();

    for(const auto &result : compiler::Linter::instance().get_lint_result())
    {
        const uint32_t line   = result.line   > 0 ? result.line   - 1 : 0;
        const uint32_t col    = result.column > 0 ? result.column - 1 : 0;
        const uint32_t endCol = col + result.length;

        diagnostics.push_back({
            {"range", {
                {"start", {{"line", line}, {"character", col}}},
                {"end",   {{"line", line}, {"character", endCol}}},
            }},
            {"severity", severityToLsp(result.severity)},
            {"message",  result.message},
            {"source",   "stegnocode"},
        });
    }

    const nlohmann::json notification = {
        {"jsonrpc", "2.0"},
        {"method",  "textDocument/publishDiagnostics"},
        {"params",  {
            {"uri",         uri},
            {"diagnostics", diagnostics},
        }},
    };

    core::writeMessage(out, notification.dump());
    std::cerr << "[lsp] diagnostics publiés: " << diagnostics.size() << " erreur(s)\n";
}
}