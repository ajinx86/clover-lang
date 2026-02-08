#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

#include "cl-log.h"
#include "cl-colors.h"
#include "cl-diagnostic.h"


CL_TYPE(DiagInfo) {
    str_t str;
    str_t fmt;
};


static const DiagInfo TYPES[] = {
    [CL_DIAG_NOTE]    = { .str = "note",    .fmt = "1;30" },
    [CL_DIAG_INFO]    = { .str = "info",    .fmt = "1;37" },
    [CL_DIAG_WARNING] = { .str = "warning", .fmt = "1;33" },
    [CL_DIAG_ERROR]   = { .str = "error",   .fmt = "1;31" },
};


static void write_snippet(DiagLocation loc) {
    int width = (int)fmax(log10(loc.line) + 1, 4);
    int caret_length = (int)fmax(loc.length, 1);

    /* error line */
    printf("%*d | ", width, loc.line);
    fwrite(source_get(loc.src, loc.line_offset), 1, loc.line_length, stdout);
    putchar('\n');

    /* caret */
    printf(" %*s | ", width, "");
    printf("%*s", loc.column - 1, "");

    for (int i = 0; i < caret_length; i++) {
        putchar((i == loc.caret) ? '^' : '~');
    }

    putchar('\n');
}


void __cl_diag(DiagType type, DiagLocation loc, str_t msg, ...) {
    if (type < 0 || type >= __CL_DIAG_MAX) {
        printf("%s: invalid diag type: %d\n", __func__, type);
        return;
    }

    va_list args;

    DiagInfo info = TYPES[type];

    if (cl_fd_use_colors(STDOUT_FILENO)) {
        printf("\e[1m%s:%u:%u-%u\e[0m: \e[%sm%s:\e[0m ",
            loc.src->path, loc.line, loc.column,
            (loc.column + loc.length), info.fmt, info.str);
    } else {
        printf("%s:%u:%u-%u: %s: ", loc.src->path,
            loc.line, loc.column, (loc.column + loc.length), info.str);
    }

    va_start(args, msg);
    vprintf(msg, args);
    va_end(args);
    putchar('\n');

    if (type > CL_DIAG_NOTE) {
        write_snippet(loc);
    }
}
