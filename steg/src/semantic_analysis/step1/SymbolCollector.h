//
// Created by Roumite on 18/03/2026.
//

#pragma once

#include "ast/ASTVisitor.h"
#include "semantic_analysis/utils/SymbolTable.h"
#include "linter/Linter.h"
#include "semantic_analysis/analyzeExports.h"
#include "../utils/helpers.h"
#include "../utils/ModuleManager.h"

namespace compiler
{
    class SymbolCollector : public ASTVisitor
    {
        std::string current_function_name;
    public:
        SymbolTable table;
        std::vector<std::filesystem::path> imported_paths;

        /*
         * Utils
         */

        struct symbol_with_name
        {
            std::string name;
            std::shared_ptr<SymbolInfo> symbol;
        };

        std::optional<symbol_with_name> get_symbols_with_name(const std::string& name, const std::filesystem::path& path)
        {
            if (auto symbol = table.get(name))
                return symbol_with_name{ name, symbol };

            // Try with local function mangling
            std::string mangled = utils::mangle_function(path, name);
            if (auto symbol = table.get(mangled))
                return symbol_with_name{ mangled, symbol };

            // Try with imported function mangling
            for (const auto& imported_path : imported_paths)
            {
                mangled = utils::mangle_function(imported_path, name);
                if (auto symbol = table.get(mangled))
                    return symbol_with_name{ mangled, symbol };
            }

            return std::nullopt;
        }

        void visit(ASTMainProgramNode* node) override {
            for (const auto &import : node->imports)
                import->accept(this);
            for (const auto &file : node->files)
                file->accept(this);
            for (const auto &var : node->global_variables)
                var->accept(this);
            for (const auto &func : node->functions)
                func->accept(this);
        }

        void visit(ASTFunctionProgramNode* node) override {
            node->name = utils::mangle_function(node->path, node->name);
            current_function_name = node->name;
            if (table.contains(node->name))
                Linter::instance().report("Function already declared: " + node->name, node->token);

            std::vector<ResolvedType> param_types;
            for (auto& param : node->parameters)
                param_types.push_back(ResolvedType::from(param->type));

            table.declare(node->name, SymbolInfo{
                .kind = SymbolKind::FUNCTION,
                .type = ResolvedType::from(node->return_type),
                .param_types = param_types,
                .is_exported = node->is_exported,
                .source_file = node->token.path,
                .token = node->token
            });

            table.enter_scope();
            for (auto& param : node->parameters)
                param->accept(this);
            node->statement->accept(this);
            table.exit_scope();

            current_function_name.clear();
        }

        void visit(ASTParameterProgramNode* node) override {
            std::string mangled_name = utils::mangle_local(current_function_name, node->name);

            table.declare(mangled_name, SymbolInfo{
                .kind = SymbolKind::VARIABLE,
                .type = ResolvedType::from(node->type),
                .token = node->token
            });

            node->name = mangled_name;
        }

        void visit(ASTVariableStatement* node) override {
            std::string mangled_name;

            if (current_function_name.empty()) { // It's a global
                mangled_name = utils::mangle_global(node->token.path, node->name);
            } else { // Local
                mangled_name = utils::mangle_local(current_function_name, node->name);
            }

            if (table.contains(mangled_name))
                Linter::instance().report("Variable already declared: " + node->name, node->token);

            if (node->expression)
                node->expression->accept(this);

            table.declare(mangled_name, SymbolInfo{
                .kind  = SymbolKind::VARIABLE,
                .type  = ResolvedType::from(node->type),
                .token = node->token
            });

            node->name = mangled_name;
        }

        void visit(ASTBlockStatementNode* node) override {
            table.enter_scope();
            for (auto &stmt : node->statements)
                stmt->accept(this);
            table.exit_scope();
        }

        void visit(ASTIfStatementNode* node) override {
            node->condition->accept(this);
            node->then_statement->accept(this);
            if (node->false_statement)
                node->false_statement->accept(this);
        }

        void visit(ASTWhileStatementNode* node) override {
            node->condition->accept(this);
            node->body_statement->accept(this);
        }

        void visit(ASTForStatementNode* node) override {
            table.enter_scope();
            if (node->init_expression)
                node->init_expression->accept(this);
            if (node->condition_expression)
                node->condition_expression->accept(this);
            if (node->post_expression)
                node->post_expression->accept(this);
            node->body_statement->accept(this);
            table.exit_scope();
        }

        void visit(ASTExpressionStatement* node) override {
            node->expression->accept(this);
        }

        void visit(ASTReturnStatement* node) override {
            if (node->expression) node->expression->accept(this);
        }

        void visit(ASTFileProgramNode* node) override {
            if (table.contains(node->name))
                Linter::instance().report("File identifier already declared: " + node->name, node->token);

            const std::filesystem::path source_dir = node->token.path.parent_path();
            const std::filesystem::path exec_dir = std::filesystem::current_path();

            const std::filesystem::path path_source = source_dir / node->path;
            const std::filesystem::path path_exec   = exec_dir   / node->path;

            std::filesystem::path final_path;

            if (std::filesystem::exists(path_source)) {
                final_path = std::filesystem::absolute(path_source);
            } else if (std::filesystem::exists(path_exec)) {
                final_path = std::filesystem::absolute(path_exec);
            } else {
                Linter::instance().report(
                    "File not found: '" + node->path + "'\n"
                    "  Tried: " + path_source.string() + "\n"
                    "  Tried: " + path_exec.string(),
                    node->token,
                    LintData::Severity::ERR
                );
                final_path = std::filesystem::absolute(path_source);
            }

            node->absolute_path = final_path.lexically_normal().string();

            if (table.get(node->name) != nullptr)
            {
                Linter::instance().report(
                    "File identifier already declared: " + node->name,
                    node->token,
                    LintData::Severity::ERR
                );
            }

            table.declare(node->name, SymbolInfo{
                .kind = SymbolKind::VARIABLE,
                .type = ResolvedType::from(ASTTypeNode::Types::FILE, 0),
                .is_exported = true,
                .token = node->token
            });
        }

        void visit(ASTFilesProgramNode*) override {}

        void visit(ASTImportProgramNode* node) override {
            auto path = utils::resolve_path(node->token.path, node->path);
            if (!path) {
                Linter::instance().report("Module not found: " + node->path, node->token);
                return;
            }

            imported_paths.push_back(std::filesystem::absolute(*path));

            std::string absolute_path = std::filesystem::absolute(*path).string();
            auto &manager = ModuleManager::instance();

            if (manager.currently_parsing.contains(absolute_path))
                return;

            manager.currently_parsing.insert(absolute_path);
            auto exports = analyzeExports(absolute_path);

            if (exports) {
                manager.module_cache[absolute_path] = std::move(*exports);
            } else {
                Linter::instance().report("Failed to parse imported file: " + node->path, node->token);
            }

            manager.currently_parsing.erase(absolute_path);

            auto &cache = manager.module_cache[absolute_path];

            const auto &get_in_cache = [&](std::filesystem::path &token_path, std::string &name) -> std::optional<symbol_with_name>
            {
                if (cache.contains(name))
                    return symbol_with_name{ name, cache[name] };
                name = utils::mangle_function(token_path, name);
                if (cache.contains(name))
                    return symbol_with_name{name, cache[name]};
                return std::nullopt;
            };

            for (auto &tok : node->functions_variables) {
                auto cached_symbol = get_in_cache(*path, tok.value);
                if (cached_symbol) {
                    if (table.contains(cached_symbol->name)) {
                        Linter::instance().report("Import conflict: '" + tok.value + "' is already defined.", tok);
                    } else {
                        auto symbol = std::make_shared<SymbolInfo>(*cache[cached_symbol->name]);
                        symbol->source_file = absolute_path;
                        table.declare(cached_symbol->name, *symbol);
                    }
                } else {
                    Linter::instance().report("Symbol '" + tok.value + "' is not exported by " + node->path, tok);
                }
            }
        }

        void visit(ASTBreakStatement*) override {}
        void visit(ASTContinueStatement*) override {}
        void visit(ASTLabelStatement* node) override {}
        void visit(ASTGotoStatement* node) override {}

        void visit(ASTBinaryExpressionNode* node) override {
            node->left->accept(this);
            node->right->accept(this);
        }

        void visit(ASTUnaryExpressionNode* node) override
        {
            node->expression->accept(this);
        }

        void visit(ASTLiteralExpressionNode*) override {}

        void visit(ASTIdentifierExpressionNode* node) override {
            auto symbol_info = get_symbols_with_name(node->name, node->token.path);

            if (!symbol_info) {
                // Try with local variable mangling
                if (!current_function_name.empty()) {
                    std::string mangled = utils::mangle_local(current_function_name, node->name);
                    if (auto symbol = table.get(mangled))
                        symbol_info = symbol_with_name{ mangled, symbol };
                }

                // Try with global variable mangling
                if (!symbol_info) {
                    std::string mangled = utils::mangle_global(node->token.path, node->name);
                    if (auto symbol = table.get(mangled))
                        symbol_info = symbol_with_name{ mangled, symbol };
                }
            }

            if (symbol_info) {
                node->resolved_symbol = symbol_info->symbol;
                node->name = symbol_info->name;
            } else {
                Linter::instance().report("Undeclared identifier: " + node->name, node->token);
            }
        }

        void visit(ASTCallExpressionNode* node) override {
            node->callee->accept(this);
            node->resolved_symbol = node->callee->resolved_symbol;

            for (auto& arg : node->args) {
                arg->accept(this);
            }
        }

        void visit(ASTIndexExpressionNode* node) override {
            node->array->accept(this);
            node->index->accept(this);
        }

        void visit(ASTAssignExpressionStatement* node) override {
            node->target->accept(this);
            node->value->accept(this);
        }

        void visit(ASTAddressOfExpressionNode* node) override {
            node->expression->accept(this);
        }
        void visit(ASTDereferenceExpressionNode* node) override {
            node->expression->accept(this);
        }
        void visit(ASTTypeNode*) override {}
        void visit(ASTErrorNode*) override {}
        void visit(ASTStatementError*) override {}
    };
}
