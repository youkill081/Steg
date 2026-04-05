//
// Created by Roumite on 26/02/2026.
//


#include "./utils.h"

#include "assembler/Assembler.h"
#include "assembler/assembler_exception.h"
#include "assembler/TextParser.h"

#include <charconv>

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

    inline bool is_float(const std::string &token)
    {
        return (token.find('.') != std::string::npos) ||
            token.ends_with('f') ||
            token.ends_with('F');
    }

    bool token_is_numeric_value(const std::string& token)
    {
        if (token.empty()) return false;

        if (is_float(token))
        {
            std::string clean_token = token;
            if (clean_token.ends_with('f') || clean_token.ends_with('F')) {
                clean_token.pop_back();
            }

            float result;
            auto [ptr, ec] = std::from_chars(clean_token.data(), clean_token.data() + clean_token.size(), result);

            return ec == std::errc() && ptr == clean_token.data() + clean_token.size();
        }
        uint32_t result;
        auto [ptr, ec] = std::from_chars(token.data(), token.data() + token.size(), result);
        return ec == std::errc() && ptr == token.data() + token.size();

    }

    uint32_t token_to_numeric(const std::string& token)
    {
        if (is_float(token))
        {
            std::string clean_token = token;
            if (clean_token.ends_with('f') || clean_token.ends_with('F')) {
                clean_token.pop_back();
            }

            float result;
            auto [ptr, ec] = std::from_chars(clean_token.data(), clean_token.data() + clean_token.size(), result);

            if (ec == std::errc() && ptr == clean_token.data() + clean_token.size()) {
                return std::bit_cast<uint32_t>(result);
            }
        } else
        {
            uint32_t result;
            auto [ptr, ec] = std::from_chars(token.data(), token.data() + token.size(), result);

            if (ec == std::errc() && ptr == token.data() + token.size()) {
                return result;
            }
        }

        Linter::error("Invalid numeric value \"" + token + "\"");
    }
}
