//
// Created by Roumite on 18/02/2026.
//

#include "WriteSequence.h"

#include "Colors.h"
#include "../constant.h"
#include "Logger.h"

void WriteSequence::write_uint32(uint32_t value)
{
    for (uint32_t i = 0; i < 32u; ++i)
    {
        pixel &curr = get_next_pixel();
        write_bit_in_pixel(curr, ((value >> (31u - i)) & 0x1u) != 0u);
    }
}

void WriteSequence::write_uint8(uint8_t value)
{
    for (uint32_t i = 0; i < 8u; ++i)
    {
        pixel &curr = get_next_pixel();
        write_bit_in_pixel(curr, ((value >> (7u - i)) & 0x1u) != 0u);
    }
}

void WriteSequence::write_byte_buffer(const ByteBuffer& buffer)
{
    for (const auto &byte : buffer.data())
    {
        write_uint8(byte);
    }

}

void WriteSequence::write_sequence(const ByteBuffer& data, DataType type)
{
    ByteBuffer header = gen_header(data, type);

    write_uint32(CHECKSUM);
    write_uint32(header.get_crc32());
    write_byte_buffer(header);
    write_byte_buffer(data);

    const uint64_t n_pixels = _index_list.size();
    const uint64_t total_capacity = 8ull * 3ull * n_pixels; // 8 layers × 3 passes × n_pixels
    const uint64_t bits_written = ((_nbr_of_shuffle - 1) * 3 + _current_pass) * n_pixels
        + _current_index;

    const float pass_filling = static_cast<float>(_current_index)
        / static_cast<float>(n_pixels) * 100.f;
    const float layer_filling = static_cast<float>(_current_pass * n_pixels + _current_index)
        / static_cast<float>(3 * n_pixels) * 100.f;
    const float total_filling = static_cast<float>(bits_written)
        / static_cast<float>(total_capacity) * 100.f;

    Logger::log(
        "-> Layer " + std::to_string(_nbr_of_shuffle)
        + " | Pass " + std::to_string(_current_pass + 1) + "/3"
        + " at " + std::to_string(pass_filling) + "%"
        + " | Layer " + std::to_string(layer_filling) + "%"
        + " | Total " + std::to_string(total_filling) + "%",
        "SequenceManager", AnsiColors::Cyan);
}
