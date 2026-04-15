#pragma once

#include <nlohmann/json.hpp>
#include <iosfwd>
#include <optional>
#include <string>

namespace core
{


    std::optional<std::string> readMessage(std::istream& in);

    void writeMessage(std::ostream& out, const std::string& body);
    void sendResponse(std::ostream& out, const nlohmann::json& id, const nlohmann::json& result);
    void sendError(std::ostream& out, const nlohmann::json& id, int code, const std::string& message);

} // namespace core