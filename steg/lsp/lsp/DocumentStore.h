#pragma once
#include <string>
#include <unordered_map>
#include <optional>

namespace lsp
{
    class DocumentStore
    {
    public:
        static DocumentStore& instance()
        {
            static DocumentStore s;
            return s;
        }

        void set(const std::string& uri, const std::string& content)
        {
            _docs[uri] = content;
        }

        void remove(const std::string& uri)
        {
            _docs.erase(uri);
        }

        std::optional<std::string> get(const std::string& uri) const
        {
            auto it = _docs.find(uri);
            if(it == _docs.end()) return std::nullopt;
            return it->second;
        }

    private:
        DocumentStore() = default;
        std::unordered_map<std::string, std::string> _docs;
    };
}