#pragma once

#include <nlohmann/json.hpp>
#include <iosfwd>

namespace handlers
{
    void onSemanticTokensFull(const nlohmann::json& id, const nlohmann::json& params, std::ostream& out);
}