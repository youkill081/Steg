//
// Created by Roumite on 17/03/2026.
//

#pragma once

#include <filesystem>

namespace compiler
{
    struct LintError
    {
        enum class Severity { ERR, WARN, HINT };

        std::string message;
        std::filesystem::path file;
        uint32_t line;
        uint32_t column;
        uint32_t length;
        Severity severity;
    };

    class Linter
    {
    private:
        std::vector<LintError> _errors;
    public:
        static Linter& instance() // One linter for all compiler process
        {
            static Linter linter;
            return linter;
        }

        void report(
            const std::string& message,
            const std::filesystem::path &file,
            uint32_t line,
            uint32_t column,
            uint32_t length = 1,
            LintError::Severity severity = LintError::Severity::ERR
        ) {
            _errors.push_back({ message, file, line, column, length, severity });
        }

        const std::vector<LintError>& get_errors() const { return _errors; }
        bool has_errors() const { return !_errors.empty(); }

        void display_diagnostics() const;

        void clear() { _errors.clear(); };
    };
}
