#include <iostream>
#include <stdexcept>

#include <clover.hpp>


static void show_help() {
    std::cout << ((
        "Usage:\n"
        "  clover [options...] [-o <output>] [--] files...\n"
        "\n"
        "Compile options:\n"
        "  -o FILE          Set output file name (only in build mode)\n"
        "  -m MANIFEST      Set manifest file\n"
        "  -d               Enable debug symbols\n"
        "\n"
        "General Options:\n"
        "  -h               Displays this message and exits\n"
        "  -v               Displays program version and exits"
    )) << std::endl;

    exit(EXIT_SUCCESS);
}


static void show_version() {
    std::cout << ((
        "clover " CLOVER_VERSION " - built on " CLOVER_BUILDINFO " \n"
        "Copyright (C) 2025 Thaynan M. Silva\n"
        "\n"
        "This is free software, and you are welcome to redistribute it\n"
        "under the terms of the GNU Lesser General Public License 3.0.\n"
        "This program comes with ABSOLUTELY NO WARRANTY!\n"
        "\n"
        "License: https://www.gnu.org/licenses/lgpl-3.0.html"
    )) << std::endl;

    exit(EXIT_SUCCESS);
}


int main(int argc, const char **argv) {
    clover::OptParser cmd({
        clover::Option("-h", clover::OptionCallbackFunc(show_help)),
        clover::Option("-v", clover::OptionCallbackFunc(show_version)),
        clover::Option("-o", true),
        clover::Option("-d"),
    });

    try {
        cmd.parse(argc, argv);
    } catch (std::invalid_argument& ex) {
        std::cerr << "error: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    clover::BuildFlags flags = {
        .f_debug = cmd.get_option("-d")
    };

    try {
        if (!clover::compile(cmd.get_args(), cmd.get_value("-o").value_or("a.clx"), flags)) {
            std::cerr << "\ncompilation failed" << std::endl;
        }
    } catch (std::runtime_error& ex) {
        std::cerr << "error: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
