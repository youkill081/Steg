//
// Created by Roumite on 23/02/2026.
//

#pragma once

#include <cstdint>
#include <string>
#include <set>
#include <map>

#include "assembler/ISymbolSource.h"
#include "assembler/Linter.h"

namespace assembler
{
    struct ParsedLine;

    struct Variable
    {
        std::string name;
        std::vector<uint16_t> value;

        bool operator<(const Variable &other) const {
            return name < other.name;
        }
    };

    class VariableSet : private std::set<Variable>, public ISymbolSource
    {
        using Base = std::set<Variable>;
    private:
        std::map<Variable, uint16_t> variables_address;
        uint16_t current_address = 0;

        static std::vector<uint16_t> token_to_data(const std::string &token, const std::string &variableName);
        static Variable parsed_line_to_variable(const ParsedLine &line);
    public:
        void push_variable(const std::string &name, const std::vector<uint16_t> &value);
        void push_variable(const Variable &new_variable);
        [[nodiscard]] uint16_t get_variable_address(const Variable &variable) const;
        [[nodiscard]] const Variable &get_variable_by_name(const std::string &name) const;
        [[nodiscard]] bool contains_variable_by_name(const std::string &name) const;
        void display() const;

        static VariableSet from_parsed_lines(const std::vector<ParsedLine> &lines, Linter &);
        [[nodiscard]] SymbolSet get_symbols() const override;

        using Base::begin;
        using Base::end;
        using Base::size;
    };
}
