//
// Created by Roumite on 26/02/2026.
//

#pragma once

#include <vector>

#include "FileSet.h"
#include "../TextParser.h"

namespace assembler
{
    struct Subtexture
    {
        const File &origin_file;
        std::string name;
        uint16_t descriptor;
        uint16_t x;
        uint16_t y;
        uint16_t width;
        uint16_t height;
    };

    class SubtexturesSet : private std::vector<Subtexture>, public ISymbolSource
    {
        using Base = std::vector<Subtexture>;
    private:
        static const File &get_file(const std::string &name, const FileSet& files) ;
        void push_subtexture_from_parsed_line(const ParsedLine &line, const FileSet& files);
    public:
        static SubtexturesSet from_parsed_lines(
            const std::vector<ParsedLine> &lines,
            const FileSet &files
        );
        [[nodiscard]] SymbolSet get_symbols() const override;

        using Base::begin;
        using Base::end;
        using Base::size;
    };
}
