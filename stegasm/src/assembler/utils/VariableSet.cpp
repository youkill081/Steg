//
// Created by Roumite on 23/02/2026.
//

#include "VariableSet.h"
#include "utils.h"
#include <charconv>
#include <iostream>
#include <utils.h>

#include "assembler/Assembler.h"
#include "assembler/TextParser.h"

using namespace assembler;

std::vector<uint16_t> VariableSet::token_to_data(const std::string &token, const std::string &variableName)
{
    uint16_t result;
    auto [ptr, ec] = std::from_chars(token.data(), token.data() + token.size(), result);

    if (ec == std::errc())
        return {result};
    if (ec == std::errc::invalid_argument)
    {
        std::vector<uint16_t> end_vector;
        for (const auto &c : token)
            end_vector.push_back(static_cast<uint16_t>(c));
        return end_vector;
    }
    if (ec == std::errc::result_out_of_range)
        Linter::error("Value \"" + token + "\" for variable \"" + variableName + "\" is out of range");
    Linter::error("Failed to parse token \"" + token + "\" for variable \"" + variableName + "\"");
}

Variable VariableSet::parsed_line_to_variable(const ParsedLine &line)
{
    if (line.tokens.size() < 2)
        Linter::error("Number of token missmatch for variable declaration");

    std::vector<uint16_t> data;
    std::string variable_name = line.tokens[0];
    for (int i = 1; i < line.tokens.size(); i++)
    {
        auto token_data = token_to_data(line.tokens[i], variable_name);
        data.insert(data.end(), token_data.begin(), token_data.end());
    }
    data.push_back('\0');

    return { .name = variable_name, .value = data };
}

VariableSet VariableSet::from_parsed_lines(const std::vector<ParsedLine> &lines, Linter &linter)
{
    const auto variables_lines = get_section_lines(lines, VARIABLE_SECTION_NAME);
    if (variables_lines.empty())
        return {};

    VariableSet variables;
    linter.foreach(variables_lines, [&](const ParsedLine &line)
    {
        variables.push_variable(parsed_line_to_variable(line));
    });
    return variables;
}

SymbolSet VariableSet::get_symbols() const
{
    SymbolSet symbols;
    for (const auto &var : *this)
        symbols.insert_symbol(var.name, get_variable_address(var), SymbolType::Variable);
    return symbols;
}

void VariableSet::push_variable(const std::string &name, const std::vector<uint16_t> &value)
{
    Variable new_variable = {name, value};
    this->push_variable(new_variable);
}

void VariableSet::push_variable(const Variable &new_variable)
{
    if  (this->contains(new_variable))
        Linter::error("Duplicate variable \"" + new_variable.name + "\" !");
    this->insert(new_variable);
    this->variables_address[new_variable] = current_address;
    current_address += new_variable.value.size();
}

uint16_t VariableSet::get_variable_address(const Variable &variable) const
{
    if (not this->variables_address.contains(variable))
        Linter::error("Variable \"" + variable.name + "\" not found !");
    return this->variables_address.at(variable);
}

const Variable &VariableSet::get_variable_by_name(const std::string &name) const
{
    for (const auto &variable : *this)
        if (variable.name == name)
            return variable;
    Linter::error("Variable \"" + name + "\" not found !");

}

bool VariableSet::contains_variable_by_name(const std::string &name) const
{
    for (const auto &variable : *this)
        if (string_equal(variable.name, name))
            return true;
    return false;
}

void VariableSet::display() const
{
    for (const auto &variable : *this)
    {
        std::cout << variable.name << " -> ";
        for (const auto &data : variable.value)
            std::cout << data << " ";
        std::cout << std::endl;
    }
}