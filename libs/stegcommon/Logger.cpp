//
// Created by Roumite on 17/02/2026.
//

#include "Logger.h"

#include <iostream>
#include <ostream>
#include <string>

#include "Colors.h"

void Logger::log(
    const std::string &message,
    const std::string &from,
    const char *from_color,
    const char *message_color)
{
    if (from.empty())
    {
        std::cout << message << std::endl;
    } else
    {
        std::cout << from_color << "[" << from << "] " << AnsiColors::Reset << message_color << message <<  AnsiColors::Reset << std::endl;
    }

}

void Logger::log_uint16_as_bit(uint16_t number)
{
    for (int i = 15; i >= 0; --i)
    {
        std::cout << ((number >> i) & 1u);
    }
    std::cout << '\n';
}

void Logger::log_uint32_as_bit(uint32_t number)
{
    for (int i = 31; i >= 0; --i)
    {
        std::cout << ((number >> i) & 1u);
    }
    std::cout << '\n';
}

void Logger::log_uint64_as_bit(uint64_t number)
{
    for (int i = 63; i >= 0; --i)
    {
        std::cout << ((number >> i) & 1u);
    }
    std::cout << '\n';
}
