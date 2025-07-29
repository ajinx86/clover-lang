#pragma once

#include <string>
#include <cstdint>
#include <iostream>
#include <optional>
#include <iomanip>
#include <cmath>


#include <base/source.hpp>


namespace clover
{

    using std::operator""s;

    namespace diagnostic
    {

        struct Format {

            std::string level;
            std::string color;

            inline std::string fmt() const {
                return color + level + "\e[0m"s;
            }

        };


        struct Location
        {
            uint32_t offset;
            uint32_t line_offset;
            uint32_t length;

            uint32_t line;
            uint32_t column;

            std::string scope; // diagnostic scope, e.g.: "function 'void example(int value)'"

            const Source& src;
        };


        const bool use_colors = ([] {
            char *env = getenv("TERM");

            if (env == nullptr)
                return false;
            if (strstr(env, "xterm") != nullptr)
                return true;
            if (strstr(env, "-256color") != nullptr)
                return true;

            return false;
        })();


        static void diag(const Format& f, const std::string& msg, const Location& l)
        {
            if (use_colors)
            {
                if (!l.scope.empty())
                {
                    std::cerr << "\e[1m" << l.src.file() << ":" << l.line << ":" << l.column << "\e[0m:";
                    std::cerr << " " << l.scope;
                    std::cerr << "\n";
                }

                std::cerr << "\e[1m" << l.src.file() << ":" << l.line << ":" << l.column << "-" << (l.column + l.length) << ": " << f.fmt() << ": " << msg << "\n";
            }
            else
            {
                if (!l.scope.empty())
                {
                    std::cerr << l.src.file() << ":" << l.line << ":" << l.column << "-" << (l.column + l.length);
                    std::cerr << ": " << l.scope;
                    std::cerr << ":\n";
                }

                std::cerr << l.src.file() << ":" << l.line << ":" << l.column << ": " << f.fmt() <<": " << msg << "\n";
            }

            // snippet and caret

            uint32_t caret_padding = (l.line_offset > 0) ? (l.offset - l.line_offset) : l.offset;
            int n_digits = static_cast<int>(std::log10(l.line)) + 1;

            std::cerr << " " << std::setw(4) << l.line << " | ";

            std::string snippet(l.src.substrv(l.line_offset, l.src.nlen(l.line_offset)));

            if (use_colors) {
                snippet.insert(l.column-1, f.color);
                snippet.insert(l.column-1+l.length+f.color.length(), "\e[0m");
            }

            std::cerr << snippet << "\n";

            std::cerr << " " << std::string(std::max(4, n_digits), ' ') << " | ";
            std::cerr << std::string(caret_padding, ' ');

            if (use_colors) {
                std::cerr << f.color;
            }

            std::cerr << "^" << std::string(l.length-1, '~');

            if (use_colors) {
                std::cerr << "\e[0m";
            }

            std::cerr << std::endl;
        }


        inline void info(const std::string& msg, const Location& l)
        {
            diag({ "info", "" }, msg, l);
        }

        inline void note(const std::string& msg, const Location& l)
        {
            diag({ "note", "\e[1m35m" }, msg, l);
        }

        inline void warn(const std::string& msg, const Location& l)
        {
            diag({ "warning", "\e[1;33m" }, msg, l);
        }

        inline void error(const std::string& msg, const Location& l)
        {
            diag({ "error", "\e[1;31m" }, msg, l);
        }

    }

}
