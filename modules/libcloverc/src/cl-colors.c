#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cl-colors.h"


bool cl_fd_use_colors(int fd) {
    static bool initialized = false;
    static bool color_always = false;
    static bool term_color = false;

    if (!initialized) {
        char *env_term = getenv("TERM");
        term_color = env_term && (
            strstr(env_term, "xterm") ||
            strstr(env_term, "-256color"));

        char *env_colors = getenv("CL_COLORS");
        color_always = env_colors && strcmp(env_colors, "1") == 0;

        initialized = true;
    }

    return color_always || (isatty(fd) && term_color);
}
