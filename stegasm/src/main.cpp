//
// stegasm - Steganography Assembly
//

#include <iostream>
#include "ByteBuffer.h"
#include "../../steganographer/images/pixel.h"
#include "compiler/Compiler.h"
#include "compiler/TextParser.h"

int main(int argc, char* argv[])
{
    try
    {
        auto binary = Compiler::compile(argv[1]);
        std::cout << binary << std::endl;
    } catch (std::exception &e)
    {
        std::cerr << "Error -> " << e.what() << std::endl;
    }

    return 0;
}
