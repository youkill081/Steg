cmake_minimum_required(VERSION 4.0)

project(steg)

set(CMAKE_CXX_STANDARD 23)

set(STEG_LIB_SOURCES
        src/main.cpp
        src/lexer/lexer_definitions.h
        src/lexer/Lexer.cpp
        src/lexer/Lexer.h
        src/lexer/TextParser.cpp
        src/lexer/TextParser.h
        src/exceptions.h
        src/exceptions.h
        src/ast/ASTNode.h
        src/ast/ASTStatementNode.cpp
        src/ast/ASTStatementNode.h
        src/ast/ASTExpressionNode.cpp
        src/ast/ASTExpressionNode.h
        src/ast/ASTProgramNode.cpp
        src/ast/ASTProgramNode.h
        src/parser/Parser.cpp
        src/parser/Parser.h
        src/parser/monadic/monadic.hpp
        src/parser/monadic/string_parser.h
)

add_library(steg_lib STATIC ${STEG_LIB_SOURCES})

target_include_directories(steg_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(steg_lib PUBLIC stegcommon)
target_link_libraries(steg_lib PUBLIC raylib)

add_executable(steg src/main.cpp)

target_link_libraries(steg PRIVATE steg_lib)