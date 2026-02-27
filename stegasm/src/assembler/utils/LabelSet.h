//
// Created by Roumite on 26/02/2026.
//

#pragma once

#include <unordered_map>
#include <vector>
#include <span>

#include "assembler/ISymbolSource.h"

namespace assembler
{
    struct ParsedLine;

    class LabelSet : public ISymbolSource
    {
    private:
        std::unordered_map<std::string, uint16_t> labels;

        static bool is_label(const ParsedLine &line);
    public:
        [[nodiscard]] SymbolSet get_symbols() const override;

        static LabelSet from_parsed_lines(const std::vector<ParsedLine> &lines);
    };
}
