//
// Created by Roumite on 18/02/2026.
//

#pragma once
#include <cstdint>

#include "Random32.h"
#include "../images/pixel.h"

class Image;

class SequenceManager
{
protected:
    Random32 random{};
    uint32_t padding = 0;

    Image &_image;
    uint32_t _seed;

    pixel &get_next_pixel();
    static uint8_t &get_pixel_data_channel(pixel &pixel);

    static void write_bit_in_pixel(pixel &pixel, bool bit);
    static bool read_bit_in_pixel(pixel &pixel);
public:
    SequenceManager(Image &image, uint32_t seed) : _image(image), _seed(seed)
    {
        random.reseed(_seed);
    }
};
