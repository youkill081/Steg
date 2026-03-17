#include "Exit.h"

#include <iostream>

namespace handlers
{

    bool onExit()
    {
        std::cerr << "[lsp] exit reçu, arrêt du serveur.\n";
        return false;
    }

}