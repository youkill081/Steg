//
// Created by Roumite on 26/02/2026.
//

#include "./Symbol.h"

using namespace assembler;

void SymbolSet::insert_symbol(const std::string &name, uint16_t value, SymbolType type)
{
    Base::insert_or_assign(name, Symbol{ value, type });
}

SymbolSet SymbolSet::operator+(const SymbolSet &other) const
{
    SymbolSet result = *this;
    for (const auto &[name, symbol] : other)
        result.insert_symbol(name, symbol.value, symbol.type);
    return result;
}