#ifdef _WIN32
#  include <fcntl.h>
#  include <io.h>
#endif

#include "core/Server.h"

#include <iostream>

int main()
{
    try
    {
#ifdef _WIN32
        _setmode(_fileno(stdin),  _O_BINARY);
        _setmode(_fileno(stdout), _O_BINARY);
#endif

        std::cerr << "[lsp] stegnocode-lsp démarré (stdio)\n";
        core::runServer(std::cin, std::cout);
        std::cerr << "[lsp] arrêt propre.\n";
    }
    catch(const std::exception& e)
    {
        std::cerr << "[lsp] FATAL: " << e.what() << '\n';
        return 1;
    }

    return 0;
}