#include <clover/log.hpp>

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>


static const struct log_fmt {
    clv_str level;
    clv_str format;
} log_fmts[] = {
    { "???",     "\e[1;40m%s\e[0m: " },
    { "debug",   "\e[1;35m%s\e[0m: " },
    { "info",    "\e[1m%s\e[0m: "    },
    { "warning", "\e[1;33m%s\e[0m: " },
    { "error",   "\e[1;31m%s\e[0m: " }
};


void
_clv_log0 (clv_str file, int lineno, int level, int mode, clv_str msg, ...) {
    va_list args;

    FILE *out;
    struct log_fmt fmt;

    if (level == CLV_DEBUG && !clv_log_debug ()) {
        return;
    }

    out = (level >= CLV_ERROR) ? stderr : stdout;
    fmt = log_fmts[(level >= CLV_DEBUG && level <= CLV_ERROR) ? 1 + level : 0];

#if defined (CLV_DEBUG) && CLV_DEBUG
    fprintf (out, "[%s:%u] ", file, lineno);
#endif /* CLV_DEBUG */

    if (mode & CLV_LOG_FORMAT) {
        fprintf (out, fmt.format, fmt.level);
    }

    va_start (args, msg);
    vfprintf (out, msg, args);
    va_end (args);

    if (mode & CLV_LOG_NEWLINE) {
        fputc ('\n', out);
    }
}


bool
clv_log_debug () {
    static int debug = -1;

    if (debug < 0) {
        char *debug_env = getenv("DEBUG");

        if (debug_env != NULL) {
            debug = (strcmp (debug_env, "1") == 0);
        } else {
            debug = false;
        }
    }

    return (bool)debug;
}
