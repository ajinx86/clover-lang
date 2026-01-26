#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "cl-log.h"


CL_TYPE(LogInfo) {
    str_t str;
    str_t fmt;
};


static const LogInfo LEVELS[] = {
    [CL_LOG_DEBUG]   = { .str = "debug",   .fmt = "1;30" },
    [CL_LOG_INFO]    = { .str = "info",    .fmt = "1;37" },
    [CL_LOG_WARNING] = { .str = "warning", .fmt = "1;33" },
    [CL_LOG_ERROR]   = { .str = "error",   .fmt = "1;31" },
    [CL_LOG_FATAL]   = { .str = "fatal",   .fmt = "1;95" },
};


static bool use_colors(int fd) {
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


void __cl_log(LogLevel level, str_t scope, str_t msg, ...) {
    if (level < 0 || level >= __CL_LOG_MAX) {
        dprintf(STDERR_FILENO, "%s: invalid log level: %d\n", __func__, level);
        return;
    }

    va_list args;

    const LogInfo info = LEVELS[level];
    const int fd = (level >= CL_LOG_ERROR) ? STDERR_FILENO : STDOUT_FILENO;

    if (scope) {
        if (use_colors(fd)) {
            dprintf(fd, "\e[1m%s:\e[0m ", scope);
        } else {
            dprintf(fd, "%s: ", scope);
        }
    }

    if (use_colors(fd)) {
        dprintf(fd, "\e[%sm%s:\e[0m ", info.fmt, info.str);
    } else {
        dprintf(fd, "%s: ", info.str);
    }

    va_start(args, msg);
    vdprintf(fd, msg, args);
    va_end(args);
}
