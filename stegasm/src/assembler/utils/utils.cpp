//
// Created by Roumite on 26/02/2026.
//


#include "./utils.h"

#include "assembler/Assembler.h"
#include "assembler/assembler_exception.h"
#include "assembler/TextParser.h"

namespace assembler
{
    std::span<const ParsedLine> get_section_lines(
        const std::vector<ParsedLine>& lines,
        const std::string &sectionName
    ) {
        bool is_in_section = false;
        uint32_t section_start_index = 0, number_line_section = 0, current_index = 0;

        for (const auto &line : lines)
        {
            if (not is_in_section &&
                line.tokens[0] == SECTION_KEYWORD &&
                line.tokens.size() >= 2 &&
                line.tokens[1] == sectionName)
            {
                if (current_index == lines.size() - 1)
                    return {};
                is_in_section = true;
                section_start_index = current_index;
            } else if (is_in_section)
            {
                if (line.tokens[0] == SECTION_KEYWORD)
                {
                    return {lines.data() + section_start_index + 1, number_line_section};
                }
                line.line_number_in_section = 1;
                number_line_section += 1;
            }

            current_index += 1;
        }

        if (number_line_section != 0)
            return {lines.data() + section_start_index + 1, number_line_section};

        return {};
    }

    bool token_is_uint32_value(const std::string& token)
    {
        uint32_t result;
        auto [ptr, ec] = std::from_chars(token.data(), token.data() + token.size(), result);
        return ec == std::errc();
    }

    uint32_t token_to_uint32(const std::string& token)
    {
        if (token_is_uint32_value(token))
        {
            uint32_t result;
            std::from_chars(token.data(), token.data() + token.size(), result);
            return result;
        }
        Linter::error("Invalid uint16 value \"" + token + "\"");
    }
}
