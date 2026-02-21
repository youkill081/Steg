//
// Created by Roumite on 20/02/2026.
//

#pragma once

#include <string>

constexpr bool isEqual(std::string_view a, std::string_view b)
{
    if (a.size() != b.size())
        return false;

    return std::equal(a.begin(), a.end(), b.begin(),
        [](const unsigned char c1, const unsigned char c2)
        {
            return std::tolower(c1) == std::tolower(c2);
        });
}