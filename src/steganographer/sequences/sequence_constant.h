//
// Created by Roumite on 18/02/2026.
//

#pragma once

#include <cstdint>

constexpr uint32_t CHECKSUM = 0b01101101011010010111010001100101;

struct header {
    std::uint32_t data_size;
};
