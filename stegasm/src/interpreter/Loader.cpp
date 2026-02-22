//
// Created by Roumite on 21/02/2026.
//

#include "Loader.h"

void Loader::init_variables(ByteBuffer& buffer, Runtime& runtime)
{
    const uint32_t number_of_variable = buffer.read_uint32();

    for (int i = 0; i < number_of_variable; i++)
    {
        const uint16_t address = buffer.read_uint16();
        const uint16_t size = buffer.read_uint16();

        runtime.memory.allocate_at(address, size); // Allocate memory

        for (int y = 0; y < size; y++)
            runtime.memory.write(address + y, buffer.read_uint16());
    }
}

Runtime Loader::load(ByteBuffer& buffer)
{
    Runtime runtime;

    buffer.reset_cursor();
    init_variables(buffer, runtime);
    return runtime;
}
