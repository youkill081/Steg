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
    auto result = compiler::compile("C:/Users/Roumite/CLionProjects/stegnocode/steg/examples/test.steg");
    if (!compiler::Linter::instance().has_errors())
    {
        std::cout << "Compilation successful" << std::endl;
        return 0;
    }
    std::cerr << "Errors during compilation" << std::endl;
    return 1;
}
