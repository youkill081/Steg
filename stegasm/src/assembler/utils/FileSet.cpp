//
// Created by Roumite on 23/02/2026.
//

#include "assembler/assembler_exception.h"
#include "FileSet.h"
#include "utils.h"
#include "assembler/Assembler.h"
#include "assembler/TextParser.h"

#include <filesystem>
#include <fstream>

using namespace assembler;

uint16_t FileSet::get_next_descriptor()
{
    static uint16_t current_descriptor = 0; // Ensure counter is the same for all FileSet instances
    return ++current_descriptor;
}

void FileSet::push_file(
    const std::string &user_name,
    const std::string &path,
    const std::string &extension
) {
    File new_file = {user_name, path, extension, get_next_descriptor()};
    if (not std::filesystem::exists(path))
        Linter::error("File at path \"" + path + "\" not found !");
    std::ifstream file_stream(path, std::ios::binary);
    int byte = 0;
    while ((byte = file_stream.get()) != EOF)
        new_file.file_data.write_uint8(byte);
    if (this->contains(new_file))
        Linter::error("Duplicate file  \"" + new_file.user_name + "\" !");
    this->insert(new_file);
}

void FileSet::push_file_from_parsed_line(const ParsedLine &line)
{
    if (line.tokens.size() != 2)
        Linter::error("Number of token missmath on file declaration");
    push_file(
        line.tokens[0],
        line.tokens[1],
        std::filesystem::path(line.tokens[1]).extension().string().substr(1)
    );
}

FileSet FileSet::from_parsed_lines(const std::vector<ParsedLine> &lines, Linter &linter)
{
    const auto files_lines = get_section_lines(lines, FILES_SECTION_NAME);
    if (files_lines.empty())
        return {};

    FileSet files{};
    linter.foreach(files_lines, [&](const ParsedLine &line)
    {
        files.push_file_from_parsed_line(line);
    });
    return files;
}

SymbolSet FileSet::get_symbols() const
{
    SymbolSet symbols;
    for (const auto &file : *this)
        symbols.insert_symbol(file.user_name, file.descriptor, SymbolType::File);
    return symbols;
}