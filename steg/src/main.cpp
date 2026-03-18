//
// Created by Roumite on 14/03/2026.
//

#include <iostream>

#include "compiler.h"
#include "ast/ASTProgramNode.h"
#include "lexer/Lexer.h"
#include "lexer/TextParser.h"
#include "linter/Linter.h"
#include "parser/parser_program.h"
#include "parser/monadic/monadic.hpp"
#include "semantic_analysis/step1/SymbolCollector.h"


int main()
{
    compiler::TextParser parser = compiler::TextParser::from_file("C:/Users/Roumite/CLionProjects/stegnocode/steg/examples/test.steg");

    auto result = compiler::analyze(parser);
    if (!compiler::Linter::instance().has_errors())
    {
        result->ast->display(0);
    } else
    {
        compiler::Linter::instance().display_diagnostics();
    }

    return 0;
}
