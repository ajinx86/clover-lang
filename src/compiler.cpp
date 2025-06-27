#include <clover/compiler.hpp>
#include <clover/source.hpp>
#include <clover/assert.hpp>
#include <clover/log.hpp>

#include <clover/lexer.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>


static void
dump_tokens (clv_source_t *source, clv_list_t *tokens) {
    clv_list_iter_t iter = clv_list_get_head (tokens);

    for (; iter != NULL; iter = clv_list_iter_get_next (iter)) {
        clv_token_t *token = static_cast<clv_token_t *>(clv_list_iter_get_data (iter));

        printf ("[%-18p]  prev: %-18p  next: %-18p  ", iter,
                clv_list_iter_get_prev (iter),
                clv_list_iter_get_next (iter));
        fflush (stdout);
        fwrite (clv_source_offset (source, token->offset), 1, token->length, stdout);
        putc ('\n', stdout);
    }
}


static bool
compile_unit (clv_str file, clv_str *out_objfile) {
    clv_assert (file != NULL, return false);
    clv_assert (out_objfile != NULL, return false);

    clv_source_t *src = clv_source_new (file);

    if (src == NULL) {
        clv_error ("%s: %s", strerror (errno), file);
        return false;
    }

    clv_list_t *tokens;
    bool result = true;

    if (!clv_lex (src, &tokens)) {
        result = false;
        goto cleanup;
    }

    dump_tokens (src, tokens);

cleanup:
    clv_source_free (src);

    return result;
}


static bool
write_exec (clv_str _manifest, clv_str output) {
    clv_error ("%s: stub", __func__);
    return false;
}


bool
clv_compile (clv_str _manifest, clv_list_t *files, clv_str output, bool debug) {
    clv_list_t *units = clv_list_new ();

    if (units == NULL) {
        return false;
    }

    bool good = true;

    clv_list_iter_t iter = clv_list_get_head (files);

    for (; iter != NULL; iter = clv_list_iter_get_next (iter)) {
        clv_str file = static_cast<clv_str>(clv_list_iter_get_data (iter));
        clv_str obj_file = NULL;

        if (compile_unit (file, &obj_file)) {
            clv_list_push_back (units, CLV_VOIDPTR (obj_file));
        } else {
            good = false;
            break;
        }
    }

    if (good && !write_exec (_manifest, output)) {
        good = false;
    }

    clv_list_free (units, (clv_list_func_t)free);

    return good;
}
