//
// Created by Roumite on 21/02/2026.
//

#pragma once

#define MAX_VALUE_IN_UINT16 65535

#include <array>
#include <vector>

enum BlockType
{
    FREE,
    USED
};

struct MemoryBlock
{
    uint16_t start;
    uint16_t size;
    BlockType free;
};

class MemoryBlockSet
{
private:
    std::vector<MemoryBlock> blocks = {{0, MAX_VALUE_IN_UINT16, FREE}};

    void merge_all_free_block();
    [[nodiscard]] uint16_t find_address_block_index(uint16_t address) const;
    [[nodiscard]] uint16_t find_free_block_index(uint16_t size) const;
public:
    [[nodiscard]] const std::vector<MemoryBlock>& get_blocks() const { return blocks; }

    [[nodiscard]] bool is_address_free(uint16_t address) const;
    [[nodiscard]] bool is_address_used(uint16_t address) const;

    uint16_t allocate(uint16_t size);
    void allocate_at(uint16_t address, uint16_t size);

    void free(uint16_t address);
};

class Memory
{
private:
    std::array<uint16_t, MAX_VALUE_IN_UINT16> _memory{};
    MemoryBlockSet _blocks;
public:
    Memory() = default;

    uint16_t read(uint16_t address) const;
    void write(uint16_t address, uint16_t value);

    uint16_t allocate(uint16_t size);
    void allocate_at(uint16_t address, uint16_t size);
    void free(uint16_t address);

    void display() const;
};