#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <string_view>


namespace clover
{

    class Source {
    public:
        Source(const std::string& file_path) : _file_path(file_path), _data(read_file(file_path)) {}

        std::string_view substrv(size_t off, size_t len) const {
            if ((off + len) >= length())
            {
                std::__throw_out_of_range_fmt("substring range [%zu..%zu] out of bounds (%zu)", off, len, length());
            }

            return std::string_view(&_data[off], len);
        }

        inline std::string substr(size_t off, size_t length) const {
            return std::string(substrv(off, length));
        }

        inline size_t span(size_t off, const std::string& accept) const {
            return strspn(&_data[off], accept.c_str());
        }

        inline size_t nspan(size_t off, const std::string& reject) const {
            return strcspn(&_data[off], reject.c_str());
        }

        inline size_t nlen(size_t line_off) const {
            return nspan(line_off, "\n");
        }

        inline int compare(size_t off, size_t len, const std::string& s) const {
            return _data.compare(off, len, s);
        }

        inline char operator[](size_t index) const { return _data[index]; }

        inline const std::string& data() const { return _data; }

        inline const std::string& file() const { return _file_path; }

        inline size_t length() const { return _data.length(); }

    private:
        static std::string read_file(const std::string& file_path) {
            std::stringstream str;
            std::ifstream fs;

            fs.open(file_path);

            if (fs.fail()) {
                std::__throw_out_of_range_fmt("failed to open file '%s': %s", file_path.c_str(), std::strerror(errno));
            }

            std::string line;

            while (getline (fs, line)) {
                str << line << "\n";
            }

            return str.str();
        }

    private:
        const std::string _file_path;
        const std::string _data;
    };

}
