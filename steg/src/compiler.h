//
// Created by Roumite on 18/03/2026.
//

#pragma once

#include "ast/ASTProgramNode.h"
#include "lexer/TokenMap.h"
#include "parser/parser_program.h"
#include "semantic_analysis/utils/SymbolTable.h"
#include "semantic_analysis/step1/SymbolCollector.h"
#include "semantic_analysis/step2/TypeInferenceVisitor.h"
#include "semantic_analysis/step3/ControlFlowVisitor.h"
#include "semantic_analysis/step4/SemanticTokensVisitor.h"


namespace compiler
{
    struct AnalysisResult {
        std::unique_ptr<ASTMainProgramNode> ast;
        SymbolTable symbols;
        TokenMap tokens;
    };

    /*
     * Analyse flow, only parse given file and return needed data for hightlight
     */
    inline std::optional<AnalysisResult> analyze(TextParser& parser)
    {
        ModuleManager::instance().clear();

        Lexer lexer(parser);
        lexer.compute();
        auto tokens = lexer.tokens();

        auto result = parseMainProgram(tokens);
        if (!result.has_value())
            return std::nullopt;

        // get all symbols
        SymbolCollector collector;
        result->value->accept(&collector);

        TokenMap token_map = lexer.get_token_map();
        SymbolTable symbols = collector.table;

        // Check all types inferences
        TypeInferenceVisitor types_inference;
        result->value->accept(&types_inference);

        ControlFlowVisitor flow;
        result->value->accept(&flow);

        // Tag all identifiers (function, function call and variable definition)
        SemanticTokensVisitor tokens_visitor(token_map, symbols);
        result->value->accept(&tokens_visitor);

        return AnalysisResult {
            .ast = std::move(result->value),
            .symbols = std::move(collector.table),
            .tokens = std::move(token_map)
        };
    }


    /* Full Compiler */


    struct CompilationUnit {
        std::filesystem::path path;
        std::unique_ptr<ASTMainProgramNode> ast;
        SymbolTable symbols;
    };

    struct CompilationResult {
        std::vector<CompilationUnit> units;
    };


    inline std::optional<CompilationUnit> compile_single(const std::filesystem::path& path)
    {
        TextParser parser = TextParser::from_file(path.string());

        Lexer lexer(parser);
        lexer.compute();
        const auto token_span = lexer.tokens();

        auto result = parseMainProgram(token_span);
        if (!result.has_value())
            return std::nullopt;

        SymbolCollector collector;
        result->value->accept(&collector);

        TypeInferenceVisitor typer;
        result->value->accept(&typer);

        ControlFlowVisitor flow;
        result->value->accept(&flow);

        return CompilationUnit {
            .path = path,
            .ast = std::move(result->value),
            .symbols = std::move(collector.table),
        };
    }

    inline std::optional<CompilationResult> compile(const std::filesystem::path &path)
    {
        Linter::instance().set_compile_mode(true);
        ModuleManager::instance().clear();

        CompilationResult out;
        std::set<std::filesystem::path> visited;

        std::function<bool(const std::filesystem::path&)> compile_recursive =
            [&](const std::filesystem::path& file) -> bool
            {
                const auto absolute = std::filesystem::absolute(file);
                if (visited.contains(absolute))
                    return true;
                visited.insert(absolute);

                auto unit = compile_single(file);
                if (!unit)
                    return false;

                TextParser sub_parser = TextParser::from_file(file.string());
                Lexer sub_lexer(sub_parser);
                sub_lexer.compute();
                auto sub_tokens = sub_lexer.tokens();
                auto sub_result = parseMainProgram(sub_tokens);
                if (sub_result) {
                    SymbolCollector sub_collector;
                    Linter::instance().disable();
                    sub_result->value->accept(&sub_collector);
                    Linter::instance().enable();

                    for (const auto& import_path : sub_collector.imported_paths)
                        if (!compile_recursive(import_path))
                            return false;
                }

                out.units.push_back(std::move(*unit));
                return true;
            };

        if (!compile_recursive(path))
            return std::nullopt;

        return out;
    }
}
