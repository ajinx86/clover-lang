#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <cl-log.h>
#include <cl-compiler.h>


#define isoption(s)     (*s == '-')
#define strcmpeq(a,b)   (strcmp(a,b) == 0)
#define prgname(s)      (strrchr(s, '/') + 1)


CL_TYPE(Options) {
    Vector *input_files;
    str_t   output_file;
};


static void show_help(str_t program) {
    printf((
        "Usage:\n"
        "  %s [option] [-o <output>] [--] files...\n"
        "\n"
        "Compile options:\n"
        "  -o FILE          Set output file name (defaults to a.co)\n"
        "\n"
        "General Options:\n"
        "  -h  --help       Shows this message and exits\n"
        "  -v  --version    Shows program version and exits\n"
    ), program);

    exit(EXIT_SUCCESS);
}


static void show_version(str_t program) {
    printf((
        "%s " CL_VERSION " (" CL_BUILDINFO ")\n"
        "Copyright (C) 2025 ajinx86\n"
        "\n"
        "This is free software, and you are welcome to redistribute it\n"
        "under the terms of the GNU Lesser General Public License 3.0.\n"
        "This program comes with ABSOLUTELY NO WARRANTY!\n"
        "\n"
        "License: https://www.gnu.org/licenses/lgpl-3.0.html\n"
    ), program);

    exit(EXIT_SUCCESS);
}


static void options_init(Options *options, int argc, str_t argv[]) {
    const str_t program = (!argv[0]) ? prgname(argv[0]) : CL_PRGNAME;

    if (argc < 2) {
        show_help(program);
    }

    options->input_files = vector_new();

    if (!options->input_files) {
        cl_fatal("%s", strerror(errno));
    }

    bool end_options = false;

    for (int i = 1; i < argc; i++) {
        str_t curr = argv[i];

        if (!isoption(curr) || end_options) {
            vector_push(options->input_files, CL_VOIDPTR(curr));
            continue;
        }

        if (strcmpeq(curr, "-h") || strcmpeq(curr, "--help")) {
            show_help(program);
        } else if (strcmpeq(curr, "-v") || strcmpeq(curr, "--version")) {
            show_version(program);
        } else if (strcmpeq(curr, "-o")) {
            if (i + 1 >= argc) {
                cl_error("missing argument for option: -o");
                exit(EXIT_FAILURE);
            }

            options->output_file = argv[++i];
        } else if (strcmpeq(curr, "--")) {
            end_options = true;
        }
    }
}


static void options_deinit(Options *options) {
    vector_free(options->input_files);
}


int main(int argc, str_t argv[]) {
    Options options;

    options_init(&options, argc, argv);

    if (!cl_compile(options.input_files, options.output_file)) {
        printf("compilation terminated.\n");
    }

    options_deinit(&options);

    return 0;
}
