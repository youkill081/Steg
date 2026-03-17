//
// Created by Roumite on 14/03/2026.
//

#include <iostream>

#include "ast/ASTProgramNode.h"
#include "lexer/Lexer.h"
#include "lexer/TextParser.h"
#include "linter/Linter.h"
#include "parser/parser_program.h"
#include "parser/monadic/monadic.hpp"

using namespace compiler;

int main()
{
    try
    {
        TextParser parser = TextParser::from_file("C:/Users/Roumite/CLionProjects/stegnocode/steg/examples/test.steg");

        Lexer lexer(parser);
        lexer.compute();
        auto tokens = lexer.tokens();

        auto result = parseMainProgram(tokens);

        if (Linter::instance().has_errors())
        {
            Linter::instance().display_diagnostics();
        } else
        {
            result->value->display(0);
        }
    } catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }


    return 0;
}
