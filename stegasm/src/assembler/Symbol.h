//
// Created by Roumite on 26/02/2026.
//

#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>

namespace assembler
{
    enum class SymbolType { Label, Variable, File, Subtexture };

    struct Symbol
    {
        uint16_t value;
        SymbolType type;
    };

    class SymbolSet : private std::unordered_map<std::string, Symbol>
    {
        using Base = std::unordered_map<std::string, Symbol>;
    public:
        void insert_symbol(const std::string &name, uint16_t value, SymbolType type);

        SymbolSet operator+(const SymbolSet &other) const;

        using Base::contains;
        using Base::at;
        using Base::begin;
        using Base::end;
        using Base::size;
    };
}