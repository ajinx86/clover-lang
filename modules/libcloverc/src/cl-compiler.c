#include "cl-compiler.h"
#include "cl-source.h"
#include "cl-log.h"
#include "cl-types.h"

#ifdef DEBUG
#include <stdio.h>
#endif


bool cl_lex(Source *src, Vector *tokens);


CL_TYPE(Unit) {
    Source *src;
    Vector *tokens;
};


static bool unit_init(Unit *self, str_t file) {
    self->tokens = vector_new(sizeof(Token));

    if (!self->tokens) {
        return false;
    }

    self->src = source_new(file);

    if (!self->src) {
        vector_free(self->tokens);
        return false;
    }

    return true;
}


static bool unit_compile(Unit *self) {
    if (!cl_lex(self->src, self->tokens)) {
        return false;
    }

#ifdef DEBUG
    /* dump tokens */
    for (size_t i = 0; i < self->tokens->count; i++) {
        Token *tk = vector_get(self->tokens, i);

        fwrite(source_get(self->src, tk->offset), 1, tk->length, stdout);
        putchar('\n');
    }
#endif /* !DEBUG */

    return true;
}


static void unit_deinit(Unit *self) {
    source_free(self->src);
    vector_free(self->tokens);
}


static bool _compile_all_units(Vector *units) {
    for (size_t i = 0; i < units->count; i++) {
        Unit *unit = vector_get(units, i);

        if (!unit_compile(unit)) {
            return false;
        }
    }

    return true;
}


bool cl_compile(Vector *files, str_t output_file) {
    Vector *units = vector_new(sizeof(Unit));

    if (!units) {
        return false;
    }

    Unit unit;
    bool success = true;

    for (size_t i = 0; i < files->count; i++) {
        str_t path = *vector_getp(files, i);

        if (!unit_init(&unit, path)) {
            success = false;
            goto cleanup;
        }

        if (!vector_push(units, &unit)) {
            success = false;
            goto cleanup;
        }
    }

    if (!_compile_all_units(units)) {
        success = false;
    }

cleanup:
    vector_iter(units, (VectorCallbackFn)unit_deinit);
    vector_free(units);

    return success;
}
