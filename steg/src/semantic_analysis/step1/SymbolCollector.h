//
// Created by Roumite on 18/03/2026.
//

#pragma once

#include "ast/ASTVisitor.h"
#include "semantic_analysis/SymbolTable.h"
#include "linter/Linter.h"

namespace compiler
{
    class SymbolCollector : public ASTVisitor
    {
    public:
        SymbolTable table;

        void visit(ASTMainProgramNode* node) override {
            for (const auto &file : node->files)
                file->accept(this);
            for (const auto &var : node->global_variables)
                var->accept(this);
            for (const auto &func : node->functions)
                func->accept(this);
        }

        void visit(ASTFunctionProgramNode* node) override {
            if (table.contains(node->name))
                Linter::instance().report("Function already declared: " + node->name, node->token);

            std::vector<ASTTypeNode::Types> param_types;
            for (auto& param : node->parameters)
                param_types.push_back(param->type->type);

            table.declare(node->name, SymbolInfo{
                .kind = SymbolKind::FUNCTION,
                .type = node->return_type->type,
                .param_types = param_types,
                .is_exported = node->is_exported,
                .token = node->token
            });

            table.enter_scope();
            for (auto& param : node->parameters)
                param->accept(this);
            node->statement->accept(this);
            table.exit_scope();
        }

        void visit(ASTParameterProgramNode* node) override {
            table.declare(node->name, SymbolInfo{
                .kind = SymbolKind::VARIABLE,
                .type = node->type->type,
                .token = node->token
            });
        }

        void visit(ASTVariableStatement* node) override {
            if (table.contains(node->name))
                Linter::instance().report("Variable already declared: " + node->name, node->token);

            table.declare(node->name, SymbolInfo{
                .kind  = SymbolKind::VARIABLE,
                .type  = node->type->type,
                .token = node->token
            });
        }

        void visit(ASTBlockStatementNode* node) override {
            table.enter_scope();
            for (auto &stmt : node->statements)
                stmt->accept(this);
            table.exit_scope();
        }

        void visit(ASTIfStatementNode* node) override {
            node->then_statement->accept(this);
            if (node->false_statement)
                node->false_statement->accept(this);
        }

        void visit(ASTWhileStatementNode* node) override {
            node->body_statement->accept(this);
        }

        void visit(ASTForStatementNode* node) override {
            table.enter_scope();
            if (node->init_expression)
                node->init_expression->accept(this);
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
            const std::filesystem::path exec_dir   = std::filesystem::current_path();

            const std::filesystem::path relative_to_source = source_dir / node->path;

            if (const std::filesystem::path relative_to_exec   = exec_dir   / node->path; !std::filesystem::exists(relative_to_source) &&
                !std::filesystem::exists(relative_to_exec))
            {
                Linter::instance().report(
                    "File not found: '" + node->path + "'\n"
                    "  Tried: " + relative_to_source.string() + "\n"
                    "  Tried: " + relative_to_exec.string(),
                    node->token,
                    LintError::Severity::ERR
                );
            }

            if (table.get(node->name) != nullptr)
            {
                Linter::instance().report(
                    "File identifier already declared: " + node->name,
                    node->token,
                    LintError::Severity::ERR
                );
            }

            table.declare(node->name, SymbolInfo{
                .kind = SymbolKind::VARIABLE,
                .type = ASTTypeNode::Types::UINT32,
                .is_exported = true,
                .token = node->token
            });
        }

        void visit(ASTFilesProgramNode*) override {}
        void visit(ASTImportProgramNode*) override {}
        void visit(ASTBreakStatement*) override {}
        void visit(ASTContinueStatement*) override {}
        void visit(ASTBinaryExpressionNode*) override {}
        void visit(ASTUnaryExpressionNode*) override {}
        void visit(ASTLiteralExpressionNode*) override {}
        void visit(ASTIdentifierExpressionNode*) override {}
        void visit(ASTCallExpressionNode*) override {}
        void visit(ASTIndexExpressionNode*) override {}
        void visit(ASTAssignExpressionStatement*) override {}
        void visit(ASTAddressOfExpressionNode*) override {}
        void visit(ASTDereferenceExpressionNode*) override {}
        void visit(ASTTypeNode*) override {}
        void visit(ASTErrorNode*) override {}
        void visit(ASTStatementError*) override {}
    };
}