//
// Created by Roumite on 18/03/2026.
//

#pragma once
#include <fstream>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#include "utils/SymbolTable.h"
#include "exports/ExportCollector.h"
#include "parser/parser_program.h"

namespace compiler {
    inline std::optional<std::unordered_map<std::string, std::shared_ptr<SymbolInfo>>> analyzeExports(const std::string path) {
        Linter::instance().disable(); // Disable lint for analyse
        TextParser parser = TextParser::from_file(path);
        Lexer lexer(parser);
        lexer.compute();
        auto tokens = lexer.tokens();

        auto result = parseMainProgram(tokens);
        if (!result.has_value())
        {
            Linter::instance().enable();
            return std::nullopt;
        }

        ExportCollector export_collector;
        result->value->accept(&export_collector);

        Linter::instance().enable();
        return export_collector.exported_symbols;
    }
}
