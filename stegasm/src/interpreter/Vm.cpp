//
// Created by Roumite on 21/02/2026.
//

#include "Vm.h"

#include "Loader.h"
#include "runtime/Runtime.h"

void Vm::run(ByteBuffer& buffer)
{
    Runtime runtime = Loader::load(buffer);

    runtime.memory.display();
    runtime.registries.display();
}
