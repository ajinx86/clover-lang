#include <clover.hpp>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>


#define CLV_OPTIONS_INIT    ((struct clv_options){ false, NULL, true, true, NULL, false, NULL, NULL })

#define isoption(x)         (strlen ((x)) >= 2 && (x)[0] == '-')
#define strequal(a,b)       (strcmp ((a), (b)) == 0)


static struct clv_options {
    bool compile_mode;
    clv_list_t *args;

    /* runtime options */

    bool rt_flag_jit;
    bool rt_flag_optimize;
    clv_str rt_exec_file;

    /* build options */

    bool cp_debug;
    clv_str cp_manifest_file;
    clv_str cp_output_file;
} options = CLV_OPTIONS_INIT;


static void
check_arity (int arity, int i, int argc, const char **argv) {
    if (i + arity >= argc) {
        clv_error ("missing argument for option '%s'. use -h to get help", argv[i]);
        exit (1);
    }
}


static void
check_compile_mode_option (clv_str option, bool toggle) {
    if (toggle && !options.compile_mode) {
        clv_error ("'%s' can only be used in combination with '-c'. use -h to get help", option);
        exit (1);
    }
}


static void
show_help () {
    printf ((
        "Usage:\n"
        "  clover [-f flag1,-flag2...] <file> [--] [args...]\n"
        "  clover -c [-d] [-m <manifest>] [-o <output>] [--] file...\n"
        "\nRun options:\n"
        "  -f FLAGS         Set runtime flags\n"
        "\nFlags:\n"
        "  - jit            Toggle Just-in-Time compiler\n"
        "  - optimize       Toggle host specific optimizations\n"
        "\nCompile options:\n"
        "  -c               Compile program\n"
        "  -d               Enable debug symbols\n"
        "  -m MANIFEST      Set manifest file\n"
        "  -o FILE          Set output file name\n"
        "\nGeneral options:\n"
        "  -h  --help       Displays this message and exits\n"
        "  -v  --version    Displays program version and exits\n"
    ));
}


static void
show_version () {
    printf ((
        "clover " CLOVER_VERSION " - built with " CLOVER_BUILDINFO " \n"
        "Copyright (C) 2025 Thaynan M. Silva\n"
        "\n"
        "This is free software, and you are welcome to redistribute it\n"
        "under the terms of the GNU Lesser General Public License 3.0.\n"
        "This program comes with ABSOLUTELY NO WARRANTY!\n"
        "\n"
        "License: <https://www.gnu.org/licenses/lgpl-3.0.html>\n"
    ));
}


static void
free_options () {
    clv_list_free (options.args, NULL);
}


static void
parse_flags (clv_str flags) {
    char *flag = strtok ((char *)flags, ",");

    bool toggle = true;

    do {
        if (flag[0] == '-') {
            toggle = false;
            flag++;
        } else {
            toggle = true;
        }

        if (strequal (flag, "jit")) {
            options.rt_flag_jit = toggle;
        } else if (strequal (flag, "optimize")) {
            options.rt_flag_optimize = toggle;
        } else {
            clv_warning ("invalid flag: '%s'", flag);
        }

        flag = strtok (NULL, " , ");
    } while (flag != NULL);
}


static void
parse_options (int argc, const char **argv) {
    bool end_options = false;

    for  (int i = 1; i < argc; i++) {
        const clv_str curr = argv[i];

        if (!end_options && isoption (curr)) {
            if (strequal (curr, "-h") || strequal (curr, "--help")) {
                show_help ();
                exit (0);
            } else if (strequal (curr, "-v") || strequal (curr, "--version")) {
                show_version ();
                exit (0);
            } else if (strequal (curr, "-c")) {
                options.compile_mode = true;
            } else if (strequal (curr, "-d")) {
                options.cp_debug = true;
            } else if (strequal (curr, "-f")) {
                check_arity (1, i, argc, argv);
                parse_flags (argv[++i]);
            } else if (strequal (curr, "-m")) {
                check_arity (1, i, argc, argv);
                options.cp_manifest_file = argv[++i];
            } else if (strequal (curr, "-o")) {
                check_arity (1, i, argc, argv);
                options.cp_output_file = argv[++i];
            } else if (strequal (argv[i], "--")) {
                end_options = true;
            } else {
                clv_error ("invalid option: '%s'. use -h to get help", argv[i]);
                exit (1);
            }
        } else {
            clv_list_push_back (options.args, CLV_VOIDPTR (argv[i]));
        }
    }

    check_compile_mode_option ("-d", (options.cp_debug));
    check_compile_mode_option ("-m", (options.cp_manifest_file != NULL));
    check_compile_mode_option ("-o", (options.cp_output_file != NULL));
}


static void
init_options () {
    options.args = clv_list_new ();

    if (!options.args) {
        perror ("failed to create args list");
        exit (1);
    }

    atexit (free_options);
}


inline static void
dump_options () {
    clv_debug ("mode: %s", options.compile_mode ? "compile" : "run");
    clv_debug ("flags: jit=%d, optimize=%d", options.rt_flag_jit, options.rt_flag_optimize);
    clv_nlog  (CLV_DEBUG, "cmdline:");

    clv_list_iter_t iter = clv_list_get_head (options.args);

    for (; iter != NULL; iter = clv_list_iter_get_next (iter)) {
        clv_str arg = static_cast<clv_str>(clv_list_iter_get_data (iter));
        clv_xlog (CLV_DEBUG, " %s", arg);
    }

    putchar ('\n');
}


inline static void
compile_program () {
    if (!clv_compile (options.cp_manifest_file, options.args, options.cp_output_file, options.cp_debug)) {
        if (errno != 0) {
            clv_error ("%s", strerror (errno));
        }

        puts ("compilation failed.");
        exit (1);
    }
}


inline static void
run_program () {
    clv_error ("not implemented: code execution");
    exit (1);
}


int
main (int argc, const char **argv) {
    if (argc < 2) {
        show_help ();
        return 1;
    }

    init_options ();
    parse_options (argc, argv);

    if (clv_log_debug ()) {
        dump_options ();
    }

    if (options.compile_mode) {
        compile_program ();
    } else {
        run_program ();
    }
}
