//
// Created by Roumite on 26/02/2026.
//

#include "LabelSet.h"

#include "utils.h"
#include "assembler/Assembler.h"
#include "assembler/TextParser.h"
#include "assembler/assembler_exception.h"

using namespace assembler;

bool LabelSet::is_label(const ParsedLine &line)
{
    return line.tokens.size() == 1 && line.tokens[0].ends_with(':');
}

SymbolSet LabelSet::get_symbols() const
{
    SymbolSet symbols;
    for (const auto &[name, address] : labels)
        symbols.insert_symbol(name, address, SymbolType::Label);
    return symbols;
}

LabelSet LabelSet::from_parsed_lines(const std::vector<ParsedLine> &lines, Linter &linter)
{
    const auto instructions_lines = get_section_lines(lines, INSTRUCTION_SECTION_NAME);
    LabelSet label_set;
    uint64_t current_instruction_idx = 0;

    linter.foreach_lines(instructions_lines, [&](const ParsedLine &line)
    {
        if (!is_label(line))
        {
            line.line_number_in_section = current_instruction_idx++;
            line.is_instruction = true;
        }
        else
        {
            line.is_instruction = false;
            std::string label_name = line.tokens[0].substr(0, line.tokens[0].size() - 1);

            if (label_set.labels.contains(label_name))
                Linter::error("Duplicate label \"" + label_name + "\" !");

            label_set.labels[label_name] = current_instruction_idx;
        }
    });
    return label_set;
}