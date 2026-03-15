//
// Created by Roumite on 14/03/2026.
//

#include <iostream>

#include "ast/ASTProgramNode.h"
#include "lexer/Lexer.h"
#include "lexer/TextParser.h"
#include "parser/monadic/monadic.hpp"
#include "parser/monadic/string_parser.h"

using namespace compilator;



enum CoucouList
{
    Coucou = 10,
    Bonjours = 20,
    Hallo = 30
};

auto parseCoucou  = map(
    parseString<"coucou">,
    [](auto) { return Coucou; }
);


auto parseBonjours  = map(
    parseString<"bonjours">,
    [](auto) { return Bonjours; }
);

auto parseHallo  = map(
    parseString<"hallo">,
    [](auto) { return Hallo; }
);

auto parseUselessChar = [](const CharStream &input) -> std::optional<Result<char, CharStream>> {

    if (input.empty() || !std::isspace(input.front())) return std::nullopt;
    return Result{ input.front(), input.subspan(1) };
};

auto skipSpaces = many(parseUselessChar);

auto parseACoucou = parseBonjours | parseCoucou | parseHallo;

auto parseAllCoucou = many1(skipSpaces >> parseACoucou << skipSpaces);

int main()
{
    try
    {
        CharStream stream = "  coucou bonjours hallo bonjours hallo   ";
        auto result = parseAllCoucou(stream);

        if (result)
        {
            for (const auto& g : result->value)
            {
                if (g == Coucou) std::cout << "Coucou" << std::endl;
                else if (g == Bonjours) std::cout << "Bonjours" << std::endl;
                else if (g == Hallo) std::cout << "Hallo" << std::endl;
                else std::cout << "Erreur de syntaxe !\n";
            }
        }
        else
        {
            std::cout << "Erreur de syntaxe !\n";
        }
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
