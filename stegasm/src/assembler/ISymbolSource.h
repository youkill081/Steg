//
// Created by Roumite on 26/02/2026.
//

#pragma once

#include "Symbol.h"

namespace assembler
{
    class ISymbolSource
    {
    public:
        [[nodiscard]] virtual SymbolSet get_symbols() const = 0;
        virtual ~ISymbolSource() = default;
    };
}