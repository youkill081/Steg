//
// Created by Roumite on 21/02/2026.
//

#pragma once

#include <map>
#include <string>

inline std::map<std::string, RegNames> stringToRegistry =
{
    {"R0", R0},
    {"R1", R1},
    {"R2", R2},
    {"R3", R3},
    {"R4", R4},
    {"R5", R5},
    {"R6", R6},
    {"R7", R7}
};

inline std::map<RegNames, std::string> registryToString =
{
    {R0, "R0"},
    {R1, "R1"},
    {R2, "R2"},
    {R3, "R3"},
    {R4, "R4"},
    {R5, "R5"},
    {R6, "R6"},
    {R7, "R7"}
};


