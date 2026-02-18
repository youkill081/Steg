//
// Created by Roumite on 18/02/2026.
//

#pragma once
#include "../SequenceManager.h"

class ReadSequence : public SequenceManager
{
private:
    void check_checksum();
public:
    ReadSequence(Image& image, uint32_t seed);
};
