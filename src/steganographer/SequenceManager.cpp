//
// Created by Roumite on 18/02/2026.
//

#include "SequenceManager.h"

#include "../images/Image.h"
#include "../utils/Logger.h"

pixel &SequenceManager::get_next_pixel()
{
    padding += random.get_next_rand();

    while (true)
    {
        pixel &pixel = _image.get_pixel(padding % _image.get_number_of_pixels());

        switch (pixel.usage)
        {
            case HIDE_SEED: ;
            case ALL_READ: padding += 1; break;
            default: return pixel;
        }
    }
}

uint8_t &SequenceManager::get_pixel_data_channel(pixel& pixel)
{
    switch (pixel.usage)
    {
    case ZERO_READ:
        pixel.usage = ONE_READ;
        return pixel.red;
    case ONE_READ:
        pixel.usage = TWO_READ;
        return pixel.blue;
    case TWO_READ:
        pixel.usage = ALL_READ;
        return pixel.green;
    default:
        throw std::runtime_error(
            "SequenceManager::get_pixel_data_uint32: try to get data on full pixel!"
        );
    }
}

void SequenceManager::write_bit_in_pixel(pixel& pixel, bool bit)
{
    uint8_t& data = get_pixel_data_channel(pixel);

    const auto b = static_cast<uint8_t>(bit ? 1u : 0u);
    data = static_cast<uint8_t>((data & 0xFEu) | b);
}

bool SequenceManager::read_bit_in_pixel(pixel& pixel)
{
    uint8_t& data = get_pixel_data_channel(pixel);
    return (data & 0x01u) != 0u;
}
