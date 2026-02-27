//
// Created by Roumite on 23/02/2026.
//

#include "InstructionSet.h"
#include "assembler/assembler_exception.h"
#include "utils/utils.hpp"
#include "assembler/TextParser.h"
#include "utils.h"
#include "assembler/Assembler.h"

#include <iostream>

using namespace assembler;

const InstructionDesc &InstructionSet::get_instruction_desc_from_parsed_line(const ParsedLine &line)
{
    std::string instruction_name = line.tokens[0];
    for (auto &instruction : instructionSet)
        if (isEqual(instruction_name, instruction.name))
            return instruction;
    throw AssemblerError("Unknown instruction \"" + instruction_name + "\"");
}

RegNames InstructionSet::string_to_reg_name(const std::string &reg_name)
{
    if (not stringToRegistry.contains(reg_name))
        throw AssemblerError("Unknown register \"" + reg_name + "\"");
    return stringToRegistry.at(reg_name);
}

UsedRegistries InstructionSet::get_used_registries_from_parsed_line(const InstructionDesc &desc, const ParsedLine &line)
{
    UsedRegistries registries;
    for (uint32_t i = 1; i < static_cast<uint32_t>(desc.regCount) + 1; i++)
        registries[i - 1] = string_to_reg_name(line.tokens[i]);
    return registries;
}

std::string InstructionSet::remove_brackets(const std::string &token)
{
    if (not token_is_in_brackets(token))
        throw AssemblerError("[userVariableWriteAsAddressToString] Invalid variable address \"" + token + "\"");
    return token.substr(1, token.size() - 2);
}

bool InstructionSet::token_is_in_brackets(const std::string &token)
{
    return token[0] == '[' && token[token.size() - 1] == ']';
}

DataValueParsingResult InstructionSet::parse_data_value(std::string token, const SymbolSet &symbols)
{
    bool is_in_bracket = token_is_in_brackets(token);
    if (is_in_bracket)
        token = remove_brackets(token);

    if (token_is_uint16_value(token))
        return { .is_address = is_in_bracket, .value = token_to_uint16(token) };

    if (!symbols.contains(token))
        throw AssemblerError("Unknown symbol \"" + token + "\"");

    return { .is_address = is_in_bracket, .value = symbols.at(token).value };
}

DataValues InstructionSet::get_data_values_from_parsed_line(
    const InstructionDesc &desc,
    const ParsedLine &line,
    const SymbolSet &symbols)
{
    DataValues data{};
    switch (desc.dataCount)
    {
    case TWO_DATA:
        data[1] = parse_data_value(line.tokens[desc.regCount + 2], symbols);
    case ONE_DATA:
        data[0] = parse_data_value(line.tokens[desc.regCount + 1], symbols);
    default:
        return data;
    }
}

Instruction InstructionSet::parsed_line_to_instruction(const ParsedLine &line, const SymbolSet &symbols)
{
    const InstructionDesc &desc = get_instruction_desc_from_parsed_line(line);
    uint32_t token_needed = static_cast<uint32_t>(desc.dataCount) + static_cast<uint32_t>(desc.regCount) + 1;

    if (line.tokens.size() != token_needed)
        throw AssemblerError("Number of token missmatch for instruction \"" + line.original_line + "\"");

    return {
        .desc = desc,
        .registries = get_used_registries_from_parsed_line(desc, line),
        .datas = get_data_values_from_parsed_line(desc, line, symbols)
    };
}

InstructionSet InstructionSet::from_parsed_lines(
    const std::vector<ParsedLine> &lines,
    const SymbolSet &symbols)
{
    auto instructions_lines = get_section_lines(lines, INSTRUCTION_SECTION_NAME, true);
    if (instructions_lines.empty())
        throw AssemblerError("No instructions in .text section !");

    InstructionSet instructions;
    for (const auto &line : instructions_lines)
    {
        if (not line.is_instruction)
            continue;
        instructions.push_back(parsed_line_to_instruction(line, symbols));
    }
    return instructions;
}


void InstructionSet::display() const
{
    for (const auto &instruction : *this)
    {
        std::cout << instruction.desc.name << " registrie(s) -> ";
        for (int i = 0; i < instruction.desc.regCount; i++)
            std::cout << registryToString.at(instruction.registries[i]) << " ";
        std::cout << "data(s) -> ";
        for (int i = 0; i < instruction.desc.dataCount; i++)
            std::cout << "\"" << instruction.datas[i].value << "\"" << (instruction.datas[i].is_address ? " as address " : " as constant ");
        std::cout << std::endl;
    }
}