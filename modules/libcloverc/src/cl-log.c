#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

#include "cl-log.h"
#include "cl-colors.h"


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


void __cl_log(LogLevel level, str_t scope, str_t msg, ...) {
    if (level < 0 || level >= __CL_LOG_MAX) {
        dprintf(STDERR_FILENO, "%s: invalid log level: %d\n", __func__, level);
        return;
    }

    va_list args;

    const LogInfo info = LEVELS[level];
    const int fd = (level >= CL_LOG_ERROR) ? STDERR_FILENO : STDOUT_FILENO;

    if (scope) {
        if (cl_fd_use_colors(fd)) {
            dprintf(fd, "\e[1m%s:\e[0m ", scope);
        } else {
            dprintf(fd, "%s: ", scope);
        }
    }

    if (cl_fd_use_colors(fd)) {
        dprintf(fd, "\e[%sm%s:\e[0m ", info.fmt, info.str);
    } else {
        dprintf(fd, "%s: ", info.str);
    }

    va_start(args, msg);
    vdprintf(fd, msg, args);
    va_end(args);
}
