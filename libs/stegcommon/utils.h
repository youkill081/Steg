//
// Created by Roumite on 22/02/2026.
//

#pragma once

#include <string>

/*
 * AI function to check if two string are equal, case_insensitive
 */
constexpr bool string_equal(const std::string_view& a, const std::string_view& b)
{
    return std::equal(a.begin(), a.end(),
                      b.begin(), b.end(),
                      [](char a, char b)
                      {
                          return std::tolower(static_cast<unsigned char>(a)) ==
                              std::tolower(static_cast<unsigned char>(b));
                      });
}
