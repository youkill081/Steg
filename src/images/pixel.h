//
// Created by Roumite on 17/02/2026.
//

#pragma once

#include <sstream>

enum pixel_usage {
    HIDE_SEED,
    ZERO_READ,
    ONE_READ,
    TWO_READ,
    ALL_READ
};

struct pixel {
    uint8_t red, blue, green;
    int usage;
};

namespace std
{
    inline std::string to_string(pixel_usage usage) {
        switch (usage) {
        case HIDE_SEED: return "HIDE_SEED";
        case ZERO_READ: return "ZERO_READ";
        case ONE_READ:  return "ONE_READ";
        case TWO_READ:  return "TWO_READ";
        case ALL_READ:  return "ALL_READ";
        default:        return "UNKNOWN";
        }
    }

    inline std::string to_string(const pixel& p) {
        std::ostringstream oss;
        oss << "R=" << static_cast<int>(p.red)
            << " G=" << static_cast<int>(p.green)
            << " B=" << static_cast<int>(p.blue)
            << " Usage=" << to_string(static_cast<pixel_usage>(p.usage));
        return oss.str();
    }
}
