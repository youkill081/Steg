//
// Created by Roumite on 21/02/2026.
//

#pragma once

#include <exception>
#include <stdexcept>

class InterpreterError : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

class MemoryError final : public InterpreterError
{
    using InterpreterError::InterpreterError;
};