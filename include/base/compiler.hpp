#pragma once

#include <vector>
#include <string>


namespace clover {

    struct BuildFlags {
        bool f_debug = false;
    };

    bool compile(const std::vector<std::string>& input_files, const std::string& out_file, BuildFlags& flags);

}
