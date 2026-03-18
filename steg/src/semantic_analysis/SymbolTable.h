//
// Created by Roumite on 18/03/2026.
//

#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <vector>

#include "../ast/ASTExpressionNode.h"

namespace compiler
{
    enum class SymbolKind {
        FUNCTION,
        VARIABLE
    };

    struct SymbolInfo {
        SymbolKind kind;
        ASTTypeNode::Types type; // For variables and functions
        std::vector<ASTTypeNode::Types> param_types; // For functions
        bool is_exported = false;
        LexerToken token; // For errors
    };

    class SymbolTable {
    public:
        std::vector<std::unordered_map<std::string, std::shared_ptr<SymbolInfo>>> scopes;

        SymbolTable() { enter_scope(); }

        void enter_scope() {
            scopes.emplace_back();
        }

        void exit_scope() {
            if (scopes.size() > 1)
                scopes.pop_back();
        }

        [[nodiscard]] bool contains_current(const std::string& name) const { // Search in current scope
            return scopes.back().contains(name);
        }

        [[nodiscard]] bool contains(const std::string& name) const { // Search in all previous scope
            for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
                if (it->contains(name)) return true;
            return false;
        }

        void declare(const std::string& name, const SymbolInfo& info) {
            scopes.back()[name] = std::make_shared<SymbolInfo>(info);
        }

        std::shared_ptr<SymbolInfo> get(const std::string &name) {
            for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
                auto found = it->find(name);
                if (found != it->end())
                    return found->second;
            }
            return nullptr;
        }
    };
}