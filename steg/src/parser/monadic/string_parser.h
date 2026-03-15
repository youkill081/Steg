//
// Created by Roumite on 15/03/2026.
//

#pragma once

#include <string_view>

#include "monadic.hpp"

namespace compilator
{
    using CharStream = std::span<const char>;

    template <size_t N>
    struct FixedString
    {
        char string[N]{};

        constexpr FixedString(const char (&str)[N]) noexcept
        {
            std::copy_n(str, N, string);
        }

        [[nodiscard]] constexpr std::string_view view() const { return { string, N - 1 }; }
        [[nodiscard]] static constexpr size_t size() { return N - 1; }
    };

    template <FixedString S>
    constexpr auto parseString = [](const CharStream input) -> std::optional<Result<std::string_view, CharStream>>
    {
        constexpr auto expected = S.view();

        if (input.size() >= expected.size() &&
            std::string_view(input.data(), expected.size()) == expected)
        {
            return Result<std::string_view, CharStream>{
                expected,
                input.subspan(expected.size())
            };
        }
        return std::nullopt;
    };

    template <char C>
    constexpr auto parseChar = [](const CharStream input) -> std::optional<Result<char, CharStream>>
    {
        if (!input.empty() && input.front() == C)
            return Result{ C, input.subspan(1) };
        return std::nullopt;
    };

    template <FixedString First, FixedString... Rest>
    constexpr auto _parseOneOfStringsImpl()
    {
        if constexpr (sizeof...(Rest) == 0)
            return parseString<First>;
        else
            return parseString<First> | _parseOneOfStringsImpl<Rest...>();
    }

    template <FixedString... Strings>
    constexpr auto parseOneOf = _parseOneOfStringsImpl<Strings...>();
}