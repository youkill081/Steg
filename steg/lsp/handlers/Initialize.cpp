#include "Initialize.h"
#include "../core/Transport.h"
#include "../lsp/Tokens.h"

namespace handlers
{
    void onInitialize(const nlohmann::json& id, const nlohmann::json& /*params*/, std::ostream& out)
    {
        const nlohmann::json result = {
            {
                "capabilities", {
                    {"positionEncoding", "utf-16"},
                    {
                        "textDocumentSync", {
                            {"openClose", true},
                            {"change", 1},
                            {"save", true},
                        }
                    },
                    {
                        "semanticTokensProvider", {
                            {
                                "legend", {
                                    {"tokenTypes", lsp::buildTokenTypesLegend()},
                                    {"tokenModifiers", nlohmann::json::array()},
                                }
                            },
                            {"full", true},
                        }
                    },
                }
            },
            {
                "serverInfo", {
                    {"name", "stegnocode-lsp"},
                    {"version", "0.1.0"},
                }
            },
        };

        core::sendResponse(out, id, result);
    }
}
