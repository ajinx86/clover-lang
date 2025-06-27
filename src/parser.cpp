#include <clover/parser.hpp>
#include <clover/assert.hpp>
#include <clover/lexer.hpp>
#include <clover/source.hpp>
#include <clover/list.hpp>

#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define PARSER_EOF                  (-1)    /* end of file */
#define PARSER_ERROR                0       /* syntax errors */
#define PARSER_FOUND                1       /* found token */
#define PARSER_NOT_FOUND            2       /* token not found, try next find function */

#define parser_at(src,index)        (clv_source_at (src, (index)))
#define parser_offset(src,offset)   (clv_source_offset (src, (offset)))


typedef struct {
    clv_source_t *src;
    clv_list_t *tokens;

    clv_token_t current;
    clv_list_iter_t current_iter;

    int offset;
    int prev_offset;
} parser_state_t;


typedef int (*clv_parser_fn_t)(parser_state_t *st, clv_node_t *parent);


static clv_node_t *
make_node (clv_node_type_t type) {
    clv_node_t *new_node = malloc (sizeof (*new_node));
    clv_assert (new_node != NULL, return NULL);

    new_node->children = clv_list_new ();
    clv_assert (new_node->children != NULL, {
        free (new_node);
        return NULL;
    });

    new_node->type = type;

    return new_node;
}


static void
parser_context (FILE *fp, clv_source_t *src, int off, int len, int line_off, int line_len, int line, int column) {
    char tmp[16] = {0};

    int num_length = snprintf (tmp, sizeof (tmp), "%3d", line);
    fprintf (fp, "%s | ", tmp);

    memset (tmp, ' ', num_length);
    fprintf (fp, "%s", tmp);

    for (int i = 0; i < column + len; i++) {
        char ch = (i < column) ? ' ' : (i == column ? '^' : '~');
        fputc (ch, fp);
    }

    fputc ('\n', fp);
}


static void
parser_error (parser_state_t *st, clv_str msg, ...) {
    va_list args;

    clv_token_t tk = *st->current;

    clv_str file = clv_source_get_file (st->src);

    fprintf (stderr, "%s:%d:%d: ", file, st->current->line, st->current->column);

    va_start (args, msg);
    vfprintf (stderr, msg, args);
    va_end (args);

    putc ('\n', stderr);

    int line_length = strcspn (parser_offset (st->src, tk.line_offset), "\r\n");

    parser_context (stderr, st->src, tk.offset, tk.length, tk.line_offset, line_length, tk.line, tk.column);
}


static inline void
parser_commit (parser_state_t *st) {
    st->prev_offset = st->offset;
}


static bool
parser_next (parser_state_t *st) {
    if (st->offset + 1 >= clv_list_length(st->tokens)) {
        return false;
    }

    st->current_iter = clv_list_iter_get_next (st->current_iter);
    st->current = clv_list_iter_get_data (st->current_iter);
    st->offset += 1;

    return true;
}


static bool
parser_is_token (parser_state_t *st, clv_tktype_t type) {
    if (st->current == NULL) {
        return false;
    }

    return st->current->type == type;
}


static int
import_stmt (parser_state_t *st, clv_node_t *parent) {
    if (!parser_is_token(st, CLV_TOKEN_IMPORT)) {
        return PARSER_NOT_FOUND;
    }

    clv_node_t *node = make_node (CLV_NODE_TYPE_IMPORT_STMT);

    clv_list_push_back(parent->children, node);
    parser_commit(st);

    return PARSER_FOUND;
}

// static clv_node_t *
// type_decl (parser_state_t *st);

// static clv_node_t *
// let_decl (parser_state_t *st);

// static clv_node_t *
// const_decl (parser_state_t *st);

// static clv_node_t *
// fn_decl (parser_state_t *st);


static int
fallback (parser_state_t *st, clv_node_t *parent) {
    parser_error (st, "unexpected token");
    return PARSER_EOF;
}


static clv_node_t *
_root (parser_state_t *st) {
    clv_node_t *node = make_node (CLV_NODE_TYPE_ROOT);
    clv_assert (node != NULL, return NULL);

    const clv_parser_fn_t fns[] = {
        import_stmt, //type_decl, let_decl, const_decl, fn_decl
        fallback
    };

    int status = PARSER_NOT_FOUND;

    while (status != PARSER_EOF) {
        for (int i = 0; i < CLV_LENGTH (fns); i++) {
            status = fns[i] (st, node);

            if (status == PARSER_FOUND) {
                break;
            } else if (status == PARSER_NOT_FOUND) {
                continue;
            } else if (status == PARSER_ERROR) {
                return NULL;
            }
        }
    }

    return node;
}


bool
clv_parse (clv_source_t *src, clv_list_t *tokens) {
    parser_state_t st;

    const clv_node_t *root = _root (&st);
}
