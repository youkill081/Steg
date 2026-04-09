//
// Created by Roumite on 18/03/2026.
//

#pragma once

#include <filesystem>
#include <optional>

namespace compiler::utils {
    inline std::optional<std::filesystem::path> resolve_path(
        const std::filesystem::path& current_file_path,
        const std::string& target_path_str)
    {
        std::filesystem::path target(target_path_str);
        std::filesystem::path source_dir = current_file_path.parent_path();
        std::filesystem::path exec_dir = std::filesystem::current_path();

        if (std::filesystem::exists(source_dir / target)) return source_dir / target;
        if (std::filesystem::exists(exec_dir / target)) return exec_dir / target;

        return std::nullopt;
    }

    [[nodiscard]] inline std::string get_mangled_prefix(const std::filesystem::path &path)
    {
        const std::filesystem::path absolute = std::filesystem::absolute(path);
        std::string result;

        for (const auto& part : absolute)
        {
            std::string s = part.stem().string();
            for (auto& c : s)
                if (!std::isalnum(c))
                    c = '_';
            if (!s.empty())
                result += s + "__";
        }

        return result;
    }

    [[nodiscard]] inline std::string mangle_global(std::filesystem::path &path, const std::string& name)
    {
        return get_mangled_prefix(path) + "__global__" + name;
    }

    [[nodiscard]] inline std::string mangle_local(
        const std::string& current_function_name,
        const std::string& name
    ) {
        return current_function_name + "__local__" + name;
    }

    [[nodiscard]] inline std::string mangle_function(const std::filesystem::path &path, const std::string& name) {
        return get_mangled_prefix(path) + "__fn__" + name;
    }
}
