//
// Created by Roumite on 18/02/2026.
//

#pragma once

#include "../SequenceManager.h"
#include "../Header.hpp"

#include <vector>

#include "../../images/Image.h"

class ByteBuffer;

class WriteSequence : public SequenceManager
{
private:
    void write_uint32(uint32_t value);
    void write_uint8(uint8_t value);
    void write_byte_buffer(const ByteBuffer& buffer);
public:
    WriteSequence(steganographer::Image& image, uint32_t seed)
        : SequenceManager(image, seed)
    {
    }

    void write_sequence(const ByteBuffer& data, DataType type);
};

