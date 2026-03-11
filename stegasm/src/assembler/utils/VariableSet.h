//
// Created by Roumite on 23/02/2026.
//

#pragma once

#include <variant>
#include <cstdint>
#include <string>
#include <set>
#include <map>

#include "assembler/ISymbolSource.h"
#include "assembler/Linter.h"

namespace assembler
{
    struct ParsedLine;

    enum VariableTypeFlag {
        VARIABLE_UINT8 = 0b01000000, // DB
        VARIABLE_UINT16 = 0b10000000, // DW
        VARIABLE_UINT32 = 0b11000000 // DD
    };

    using VariableType = std::variant<uint8_t, uint16_t, uint32_t>;
    struct Variable
    {
        std::string name;

        VariableTypeFlag type = VARIABLE_UINT8;
        std::vector<VariableType> value;

        uint8_t flags = 0;

        bool operator<(const Variable &other) const {
            return name < other.name;
        }
    };

    class VariableSet : private std::set<Variable>, public ISymbolSource
    {
        using Base = std::set<Variable>;
    private:
        std::map<Variable, uint32_t> variables_address;
        uint32_t current_address = 0;

        static VariableTypeFlag get_variable_type(const ParsedLine& line);
        static Variable parsed_line_to_variable(const ParsedLine &line);
    public:
        void push_variable(const std::string& name, const std::vector<VariableType>& value, VariableTypeFlag type, uint8_t flags = VARIABLE_UINT8);
        void push_variable(const Variable &new_variable);
        [[nodiscard]] uint32_t get_variable_address(const Variable &variable) const;
        [[nodiscard]] const Variable &get_variable_by_name(const std::string &name) const;
        [[nodiscard]] bool contains_variable_by_name(const std::string &name) const;
        void display() const;

        static VariableSet from_parsed_lines(const std::vector<ParsedLine> &lines, Linter &);

        [[nodiscard]] SymbolSet get_symbols() const override;
        void merge(const VariableSet &other, Linter& linter);

        using Base::begin;
        using Base::end;
        using Base::size;
    };
}
