//
// Created by Roumite on 14/03/2026.
//

#pragma once

#include <exception>
#include <stdexcept>

class StegException : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

class LexerException final : public StegException
{
    using StegException::StegException;
};