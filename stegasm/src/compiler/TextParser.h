//
// Created by Roumite on 20/02/2026.

#pragma once

#include <stdexcept>
#include <vector>

#define COMMENT_CHAR ';'

class TextParserError : public std::runtime_error
{
public:
    explicit TextParserError(const std::string& Message) : runtime_error(Message) {}
};

struct ParsedLine
{
    std::size_t line_number;
    std::string original_line;
    std::vector<std::string> tokens;
};

class TextParser
{
private:
    std::string _file_path;

    static std::string remove_comments(const std::string &line);
    static std::string trim(const std::string &line);
    static std::vector<std::string> tokenize(const std::string &line);
public:
    explicit TextParser(std::string file_path);

    [[nodiscard]] std::vector<ParsedLine> parse() const;
};