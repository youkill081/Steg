//
// Created by Roumite on 17/02/2026.
//

#include "Logger.h"

#include <iostream>
#include <ostream>
#include <string>

void Logger::log(const std::string& message, const std::string& from)
{
    if (from.empty())
    {
        std::cout << message << std::endl;
    } else
    {
        std::cout << "[" << from << "] " << message << std::endl;
    }

}

void Logger::log_uint32_as_bit(uint32_t number)
{
    for (int i = 31; i >= 0; --i)
    {
        std::cout << ((number >> i) & 1u);
    }
    std::cout << '\n';
}