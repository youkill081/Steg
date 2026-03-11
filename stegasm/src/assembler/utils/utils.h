//
// Created by Roumite on 26/02/2026.
//

#pragma once

#include <span>
#include <vector>
#include <string>

#include "assembler/Linter.h"

namespace assembler
{
    struct ParsedLine;

    std::span<const ParsedLine> get_section_lines(
        const std::vector<ParsedLine>& lines,
        const std::string& sectionName
    );

    bool token_is_uint32_value(const std::string &token);
    uint16_t token_to_uint32(const std::string &token);

}
