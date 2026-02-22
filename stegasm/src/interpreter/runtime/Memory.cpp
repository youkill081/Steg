//
// Created by Roumite on 21/02/2026.
//

#include "Memory.h"

#include "Logger.h"
#include "../exceptions.h"
#include "../../../../steganographer/images/pixel.h"

// ----- MemoryBlockSet -----

uint16_t MemoryBlockSet::find_free_block_index(uint16_t size) const
{
    for (uint16_t i = 0; i < static_cast<uint16_t>(blocks.size()); ++i)
    {
        if (blocks[i].free == FREE && blocks[i].size >= size)
        {
            return i;
        }
    }
    throw MemoryError("Not enough memory to allocate " + std::to_string(size));
}

bool MemoryBlockSet::is_address_free(uint16_t address)
{
    return blocks[find_address_block_index(address)].free == FREE;
}

bool MemoryBlockSet::is_address_used(uint16_t address)
{
    return blocks[find_address_block_index(address)].free == USED;
}

uint16_t MemoryBlockSet::allocate(uint16_t size)
{
    const uint16_t index = find_free_block_index(size);

    const MemoryBlock new_block = {
        .start = blocks[index].start,
        .size = size,
        .free = USED
    };

    blocks[index].start = blocks[index].start + size;
    blocks[index].size -= size;

    blocks.insert(blocks.begin() + index, new_block);
    return new_block.start;
}

void MemoryBlockSet::allocate_at(uint16_t address, uint16_t size)
{
    if (size == 0) return;

    if (address + size > MAX_VALUE_IN_UINT16) {
        throw MemoryError("Allocation out of bounds");
    }

    uint16_t index = find_address_block_index(address);
    MemoryBlock& block = blocks[index];

    if (block.free == USED) {
        throw MemoryError("Address " + std::to_string(address) + " is already used");
    }

    if (address + size > block.start + block.size) {
        throw MemoryError("Not enough contiguous free space at this address");
    }

    uint16_t space_before = address - block.start;
    uint16_t space_after = (block.start + block.size) - (address + size);

    if (space_before > 0 && space_after > 0) {
        block.size = space_before;

        MemoryBlock used_block = {address, size, USED};
        MemoryBlock free_block_after = {static_cast<uint16_t>(address + size), space_after, FREE};

        blocks.insert(blocks.begin() + index + 1, used_block);
        blocks.insert(blocks.begin() + index + 2, free_block_after);
    }
    else if (space_before > 0 && space_after == 0) {
        block.size = space_before;

        MemoryBlock used_block = {address, size, USED};
        blocks.insert(blocks.begin() + index + 1, used_block);
    }
    else if (space_before == 0 && space_after > 0) {
        block.free = USED;
        block.size = size;

        MemoryBlock free_block_after = {static_cast<uint16_t>(address + size), space_after, FREE};
        blocks.insert(blocks.begin() + index + 1, free_block_after);
    }
    else {
        block.free = USED;
    }
}

void MemoryBlockSet::merge_all_free_block()
{
    if (blocks.empty()) return;

    for (size_t i = 0; i < blocks.size() - 1;)
    {
        if (blocks[i].free == FREE && blocks[i + 1].free == FREE)
        {
            blocks[i].size += blocks[i + 1].size;
            blocks.erase(blocks.begin() + i + 1);
        }
        else
        {
            i++;
        }
    }
}

uint16_t MemoryBlockSet::find_address_block_index(uint16_t address) const
{
    for (uint16_t i = 0; i < static_cast<uint16_t>(blocks.size()); ++i)
    {
        if (blocks[i].start <= address && address < blocks[i].start + blocks[i].size)
        {
            return i;
        }
    }
    throw MemoryError("Address " + std::to_string(address) + " is not in memory");
}

void MemoryBlockSet::free(uint16_t address)
{
    const uint16_t index = find_address_block_index(address);

    if (blocks[index].free == FREE)
        throw MemoryError("Address " + std::to_string(address) + " is already free");

    blocks[index].free = FREE;
    merge_all_free_block();
}

// ----- Memory -----

uint16_t Memory::read(uint16_t address)
{
    if (_blocks.is_address_free(address))
        throw MemoryError("[SEGFAULT] Try to read at FREE address " + std::to_string(address));
    return _memory[address];
}

void Memory::write(uint16_t address, uint16_t value)
{
    if (_blocks.is_address_free(address))
        throw MemoryError("[SEGFAULT] Try to write at FREE address " + std::to_string(address));
    _memory[address] = value;
}

uint16_t Memory::allocate(uint16_t size)
{
    return _blocks.allocate(size);
}

void Memory::allocate_at(uint16_t address, uint16_t size)
{
    _blocks.allocate_at(address, size);
}

void Memory::free(uint16_t address)
{
    _blocks.free(address);
}

void Memory::display() const
{
    Logger::log("Memory display", "Memory");
    for (const auto &block : this->_blocks.get_blocks())
    {
        std::stringstream stream;
        stream << "Block " << block.start << " - " << block.start + block.size << " : ";
        if (block.free == FREE) stream << "FREE";
        else stream << "USED";
        Logger::log(stream.str(), "Memory");

        if (block.free == USED)
        {
            std::stringstream data_stream;
            for (uint16_t i = block.start; i < block.start + block.size; ++i)
                data_stream << " " << _memory[i];
            Logger::log(data_stream.str(), "Memory");
        }
    }
}
