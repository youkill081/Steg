//
// stegasm - Steganography Assembly
//

#include <iostream>
#include "ByteBuffer.h"
#include "compiler/Compiler.h"
#include "interpreter/Vm.h"

int main(int argc, char* argv[])
{
    try
    {
        auto binary = compiler::Compiler::compile(argv[1]);
        Vm::run(binary);
    } catch (std::exception &e)
    {
        std::cerr << "Error -> " << e.what() << std::endl;
    }
    return 0;
}
