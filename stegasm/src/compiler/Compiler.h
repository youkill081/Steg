//
// Created by Roumite on 20/02/2026.
//

#pragma once

#include <iostream>
#include <utility>
#include <vector>
#include <array>
#include <span>
#include <map>

#include "../instructions.h"
#include "ByteBuffer.h"
#include "TextParser.h"
#include "utils.h"

#define SECTION_KEYWORD "section"
#define VARIABLE_SECTION_NAME ".data"
#define INSTRUCTION_SECTION_NAME ".text"

namespace compiler
{
    struct CompilerError : public std::runtime_error
    {
        explicit CompilerError(const std::string& Message) : std::runtime_error(Message) {}
    };

    /*
     * Variable Management
     */

    struct Variable
    {
        std::string name;
        std::vector<uint16_t> value;

        bool operator<(const Variable &other) const {
            if (name != other.name)
                return name < other.name;
            return value < other.value;
        }
    };

    class VariableSet : public std::vector<Variable>
    {
    private:
        std::map<Variable, uint16_t> variables_address;
        uint16_t current_address = 0;
    public:
        void push_variable(const std::string& name, const std::vector<uint16_t>& value)
        {
            Variable new_variable = {name, value};
            this->push_variable(new_variable);
        }

        void push_variable(const Variable &new_variable)
        {
            for (const auto &variable : *this)
            {
                if (string_equal(variable.name, new_variable.name))
                    throw CompilerError("Multiple variable with name " + new_variable.name + " !");
            }
            this->push_back(new_variable);

            this->variables_address[new_variable] = current_address;
            current_address += new_variable.value.size();
        }

        [[nodiscard]] uint16_t get_variable_address(const Variable &variable) const
        {
            if (not this->variables_address.contains(variable))
                throw CompilerError("[get_variable_address] Variable \"" + variable.name + "\" not found !");
            return this->variables_address.at(variable);
        }

        [[nodiscard]] const Variable &get_variable_by_name(const std::string &name) const
        {
            for (const auto &variable : *this)
                if (variable.name == name)
                    return variable;
            throw CompilerError("[get_variable_by_name] Variable \"" + name + "\" not found !");
        }

        [[nodiscard]] bool contains_variable_by_name(const std::string& name) const
        {
            for (const auto &variable : *this)
            {
                if (string_equal(variable.name, name))
                    return true;
            }
            return false;
        }

        void display() const
        {
            for (const auto &variable : *this)
            {
                std::cout << variable.name << " -> ";
                for (const auto &data : variable.value)
                {
                    std::cout << data << " ";
                }
                std::cout << std::endl;
            }
        }
    };

    /*
     * Instruction Management
     */

    using UsedRegistries = std::array<RegNames, 8>;

    struct DataValueParsingResult
    {
        bool is_address;
        uint16_t value;
    };
    using DataValues = std::array<DataValueParsingResult, 2>;

    struct Instruction
    {
        const InstructionDesc &desc;
        UsedRegistries registries;
        DataValues datas;
    };

    class InstructionSet : public std::vector<Instruction>
    {
    public:
        void display()
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
    };

    /*
     * Label management
     */

    using LabelMap = std::map<std::string, uint16_t>; // Map label's name with its associated code line

    /*
     * Compilation management
     */

    struct CompiledFile
    {
        VariableSet variables;
        InstructionSet instructions;
    };

    class Compiler
    {
    private:
        static std::span<const ParsedLine> getSectionLines(
            const std::vector<ParsedLine> &lines,
            const std::string& sectionName,
            bool throwIfNotFound = false
        );

        static std::vector<uint16_t> tokenToData(const std::string &token, const std::string &variableName);
        static Variable parsedLineToVariable(const ParsedLine &line);
        static VariableSet parseVariables(const std::vector<ParsedLine> &line);

        static const InstructionDesc &getInstructionDescFromParsedLine(const ParsedLine &line);
        static RegNames stringToRegName(const std::string &reg_name);
        static UsedRegistries getUsedRegistriesFromParsedLine(const InstructionDesc &desc, const ParsedLine &line);
        static std::string userVariableWriteAsAddressToString(const std::string &token);
        static bool userWriteVariableAsAddress(const std::string &token);
        static uint16_t tokenToUint16(const std::string &token);
        static bool tokenIsValidValue(const std::string &token);
        static DataValueParsingResult parseDataValue(std::string token, const VariableSet &variables, const LabelMap &labels);
        static DataValues getDataValuesFromParsedLine(const InstructionDesc &desc, const ParsedLine &line, const LabelMap &labels, const VariableSet &variables);
        static Instruction parsedLineToInstruction(const ParsedLine &line, const VariableSet &variables, const LabelMap &labels);
        static InstructionSet parseInstructions(const std::vector<ParsedLine> &lines, const VariableSet &variables);

        static bool is_label(const ParsedLine &line);
        static LabelMap parseLabels(const std::span<const ParsedLine> &lines, const VariableSet &variables);

        static CompiledFile compileFile(const std::string &path);

        static void writeRegXInBuffer(uint8_t reg_x, const RegCount &reg_count, const UsedRegistries &registries, ByteBuffer &buffer);
        static void writeDatasFlagInBuffer(const DataCount &data_count, const DataValues &data_parsing_result, ByteBuffer &buffer);
        static ByteBuffer compiledFileToBytebuffer(const CompiledFile &compiledFile);
    public:
        static ByteBuffer compile(const std::string &path);
    };
}
