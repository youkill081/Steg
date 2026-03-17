#include "Diagnostics.h"
#include "DocumentStore.h"
#include "Tokens.h"                       // uriToPath
#include "../core/Transport.h"

#include <iostream>

#include "lexer/Lexer.h"
#include "lexer/TextParser.h"
#include "linter/Linter.h"
#include "parser/parser_program.h"

namespace lsp
{

static int severityToLsp(compiler::LintError::Severity sev)
{
    switch(sev)
    {
        case compiler::LintError::Severity::ERR:  return 1;
        case compiler::LintError::Severity::WARN: return 2;
        case compiler::LintError::Severity::HINT: return 3;
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

        compiler::Lexer lexer(parser);
        lexer.compute();
        auto tokens = lexer.tokens();

        compiler::parseMainProgram(tokens);
    }
    catch(const std::exception& e)
    {
        std::cerr << "[lsp] diagnostics error: " << e.what() << '\n';
    }

    auto diagnostics = nlohmann::json::array();

    for(const auto& err : compiler::Linter::instance().get_errors())
    {
        const uint32_t line   = err.line   > 0 ? err.line   - 1 : 0;
        const uint32_t col    = err.column > 0 ? err.column - 1 : 0;
        const uint32_t endCol = col + err.length;

        diagnostics.push_back({
            {"range", {
                {"start", {{"line", line}, {"character", col}}},
                {"end",   {{"line", line}, {"character", endCol}}},
            }},
            {"severity", severityToLsp(err.severity)},
            {"message",  err.message},
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