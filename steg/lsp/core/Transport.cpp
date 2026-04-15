#include "Transport.h"

#include <iostream>
#include <string>

namespace core
{

    std::optional<std::string> readMessage(std::istream& in)
    {
        int contentLength = -1;

        while(true)
        {
            std::string line;
            if(!std::getline(in, line)) return std::nullopt;
            if(!line.empty() && line.back() == '\r') line.pop_back();
            if(line.empty()) break;

            constexpr std::string_view clHeader = "Content-Length: ";
            if(std::string_view(line).starts_with(clHeader))
                contentLength = std::stoi(line.substr(clHeader.size()));
        }

        if(contentLength <= 0) return std::nullopt;

        std::string body(static_cast<size_t>(contentLength), '\0');
        if(!in.read(body.data(), contentLength)) return std::nullopt;

        return body;
    }

    void writeMessage(std::ostream& out, const std::string& body)
    {
        out << "Content-Length: " << body.size() << "\r\n"
            << "\r\n"
            << body;
        out.flush();
    }

    void sendResponse(std::ostream& out, const nlohmann::json& id, const nlohmann::json& result)
    {
        const nlohmann::json response = {
            {"jsonrpc", "2.0"},
            {"id",      id},
            {"result",  result},
        };
        writeMessage(out, response.dump());
    }

    void sendError(std::ostream& out, const nlohmann::json& id, int code, const std::string& message)
    {
        const nlohmann::json response = {
            {"jsonrpc", "2.0"},
            {"id",      id},
            {"error",   {{"code", code}, {"message", message}}},
        };
        writeMessage(out, response.dump());
    }

} // namespace core