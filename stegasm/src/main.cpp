//
// stegasm - Steganography Assembly
//

#include <iostream>
#include "ByteBuffer.h"
#include "assembler/Assembler.h"
#include "interpreter/Vm.h"

int main(int argc, char* argv[])
{
    try
    {
        auto binary = assembler::Assembler::assemble(argv[1]);
        Vm::run(binary);
    } catch (std::exception &e)
    {
        std::cerr << "Error -> " << e.what() << std::endl;
    }
    return 0;
}
