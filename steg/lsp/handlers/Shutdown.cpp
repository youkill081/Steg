#include "Shutdown.h"
#include "../core/Transport.h"

namespace handlers
{
    void onShutdown(const nlohmann::json& id, const nlohmann::json& /*params*/, std::ostream& out)
    {
        core::sendResponse(out, id, nullptr);
    }
}