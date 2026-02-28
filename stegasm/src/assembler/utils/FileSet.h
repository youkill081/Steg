//
// Created by Roumite on 23/02/2026.
//

#pragma once

#include <set>
#include <string>
#include "ByteBuffer.h"

#include "assembler/ISymbolSource.h"
#include "assembler/Linter.h"

namespace assembler
{
    struct ParsedLine;

    struct File
    {
        std::string user_name;
        std::string path;
        std::string extension;
        uint16_t descriptor = 0;
        mutable ByteBuffer file_data{};

        bool operator<(const File &other) const
        {
            return user_name < other.user_name;
        }
    };

    class FileSet : private std::set<File>, public ISymbolSource
    {
        using Base = std::set<File>;
    public:
        static uint16_t get_next_descriptor();

        void push_file(const std::string &user_name, const std::string &path, const std::string &extension);
        void push_file_from_parsed_line(const ParsedLine &line);
        static FileSet from_parsed_lines(const std::vector<ParsedLine> &lines, Linter &);

        [[nodiscard]] SymbolSet get_symbols() const override;

        using Base::begin;
        using Base::end;
        using Base::size;
    };
}
