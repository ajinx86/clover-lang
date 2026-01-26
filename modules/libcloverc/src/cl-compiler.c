#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "cl-compiler.h"
#include "cl-source.h"
#include "cl-log.h"


bool cl_lex(Source *src, Vector *tokens);


CL_TYPE(Unit) {
    Source *src;
    Vector *tokens;
};


static Unit *unit_new(str_t file) {
    Unit *new_unit = malloc(sizeof(Unit));

    if (!new_unit) {
        return NULL;
    }

    new_unit->tokens = vector_new();

    if (!new_unit->tokens) {
        free(new_unit);
        return NULL;
    }

    new_unit->src = source_new(file);

    if (!new_unit->src) {
        vector_free(new_unit->tokens);
        free(new_unit);
        return NULL;
    }

    return new_unit;
}


static bool unit_compile(Unit *self) {
    cl_debug("%s: not implemented\n", __func__);
    return false;
}


static void unit_free(Unit *self) {
    source_free(self->src);
    vector_iter(self->tokens, (VectorCallbackFn)free);
    vector_free(self->tokens);
    free(self);
}


static bool _compile_all_units(Vector *units) {
    for (size_t i = 0; i < units->count; i++) {
        Unit *unit = vector_at(units, i);

        if (!unit_compile(unit)) {
            return false;
        }
    }

    return true;
}


bool cl_compile(Vector *files, str_t output_file) {
    Vector *units = vector_new();
    bool success = true;

    if (!units) {
        return NULL;
    }

    for (size_t i = 0; i < files->count; i++) {
        str_t path = (str_t)vector_at(files, i);
        Unit *unit = unit_new(path);

        if (!unit) {
            success = false;
            goto cleanup;
        }

        if (!vector_push(units, unit)) {
            success = false;
            goto cleanup;
        }
    }

    if (!_compile_all_units(units)) {
        success = false;
    }

cleanup:
    vector_iter(units, (VectorCallbackFn)unit_free);
    vector_free(units);

    return success;
}
