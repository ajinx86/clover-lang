#include <clover/lexer.hpp>
#include <clover/log.hpp>

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#define LEXER_ESCAPES           "abefnrtv\"\'\\"
#define LEXER_ESCAPES_PAIR      "\a\b\e\f\n\r\t\v\'\\"
#define LEXER_DELIMITERS        " .,:;()[]{}<>^'\"|/!?&%*-+=\r\n"

#define LEXER_DIGITS            "0123456789"

#define LEXER_ID_MAX_LENGTH     63      /* 63 */

#define LEXER_EOF               (-1)    /* end of file */
#define LEXER_ERROR             0       /* syntax errors */
#define LEXER_FOUND             1       /* found token */
#define LEXER_NOT_FOUND         2       /* token not found, try next find function */


#define lex_at(st,index)        (clv_source_at ((st)->src, (index)))
#define lex_offset(st,offset)   (clv_source_offset ((st)->src, (offset)))


typedef struct {
    clv_source_t *src;

    uint32_t offset;
    uint32_t prev_offset;
    uint32_t line_offset;

    uint32_t line;
    uint32_t column;

    struct {
        uint32_t offset;
        uint32_t prev_offset;
        uint32_t line_offset;

        uint32_t line;
        uint32_t column;
    } _save;

    bool error;
} lexer_state_t;


typedef struct {
    clv_str value;
    size_t  length;
    clv_tktype_t type;
} lexer_pair_t;


typedef int (*lexer_func_t)(lexer_state_t *st, clv_token_t *out_token);


/* == Auxiliary Functions == */


static void
lex_error (lexer_state_t *st, clv_str msg, ...) {
    va_list args;

    clv_str file = clv_source_get_file (st->src);

    int length = strcspn (lex_offset (st, st->line_offset), "\r\n");

    fprintf (stderr, "%s:%d:%d: ", file, st->line, st->column);

    va_start (args, msg);
    vfprintf (stderr, msg, args);
    va_end (args);

    putc ('\n', stderr);

    fprintf (stderr, " %3d | ", st->line);
    fwrite (lex_offset (st, st->line_offset), 1, length, stderr);
    fputc ('\n', stderr);
}


static inline void
lex_commit (lexer_state_t *st, clv_token_t *out_token, clv_tktype_t type) {
    *out_token = (clv_token_t){
        .type = type,
        .offset = st->prev_offset,
        .line_offset = st->line_offset,
        .length = st->offset - st->prev_offset,
        .line = st->line,
        .column = st->column
    };

    // commit lexer state
    st->column += st->offset - st->prev_offset;
    st->prev_offset = st->offset;
}


static inline bool
lex_equal (lexer_state_t *st, clv_str string, int length) {
    return strncmp (lex_offset (st, st->offset), string, length) == 0;
}


static inline bool
lex_arity (lexer_state_t *st, int length) {
    return st->offset + length <= clv_source_length (st->src);
}


static void
lex_save (lexer_state_t *st) {
    st->_save.offset = st->offset;
    st->_save.line_offset = st->line_offset;
    st->_save.prev_offset = st->prev_offset;
    st->_save.line = st->line;
    st->_save.column = st->column;
}


static void
lex_restore (lexer_state_t *st) {
    st->offset = st->_save.offset;
    st->line_offset = st->_save.line_offset;
    st->prev_offset = st->_save.prev_offset;
    st->line = st->_save.line;
    st->column = st->_save.column;
}


static void
lex_skip_blank (lexer_state_t *st) {
    int count;

    for (int left = 2; left > 0; left--) {
        if (!lex_arity (st, 1)) {
            break;
        }

        count = strspn (lex_offset (st, st->offset), "\t ");

        if (count > 0) {
            st->offset += count;
            st->column += count;

            left = 2;
            continue;
        }

        count = strspn (lex_offset (st, st->offset), "\r\n");

        if (count > 0) {
            st->offset += count;
            st->line_offset = st->offset;
            st->line += count;
            st->column = 1;

            left = 2;
            continue;
        }
    }

    st->prev_offset = st->offset;
}


static inline bool
lex_isalpha (char ch) {
    return ((ch >= 'a' && ch <= 'z') ||
            (ch >= 'A' && ch <= 'Z'));
}


static inline bool
lex_isalnum (char ch) {
    return ((ch >= '0' && ch <= '9') ||
            (ch >= 'a' && ch <= 'z') ||
            (ch >= 'A' && ch <= 'Z'));
}


static inline bool
lex_isdigit (char ch) {
    return (ch >= '0' && ch <= '9');
}


static inline bool
lex_isxdigit (char ch) {
    return ((ch >= '0' && ch <= '9') ||
            (ch >= 'a' && ch <= 'f') ||
            (ch >= 'A' && ch <= 'F'));
}


/* == Validation Functions == */


static bool
lex_check_escape (lexer_state_t *st) {
    char ch = lex_at (st, st->offset);

    bool found = true;

    if (strchr (LEXER_ESCAPES, ch) != NULL) {
        st->offset += 1;
    } else if (ch == 'x' || ch == 'X') { /* \xHH */
        st->offset += 1;

        for (int i = 0; i < 2; i++) {
            ch = lex_at (st, st->offset + i);

            if (!lex_isxdigit (ch)) {
                found = false;
                break;
            }
        }

        st->offset += 2;
    } else if (ch == 'u') { /* \uHHHH */
        st->offset += 1;

        for (int i = 0; i < 4; i++) {
            ch = lex_at (st, st->offset + i);

            if (!lex_isxdigit (ch)) {
                found = false;
                break;
            }
        }

        st->offset += 4;
    } else if (ch == 'U') { /* \UHHHHHHHH */
        st->offset += 1;

        for (int i = 0; i < 8; i++) {
            ch = lex_at (st, st->offset + i);

            if (!lex_isxdigit (ch)) {
                found = false;
                break;
            }
        }

        st->offset += 8;
    } else {
        found = false;
    }

    if (!found) {
        lex_error (st, "invalid escape sequence");
    }

    return found;
}


static bool
lex_check_identifier (lexer_state_t *st) {
    char ch = lex_at (st, st->prev_offset);

    if (!(lex_isalpha (ch) || ch == '_')) {
        return false;
    }

    int length = st->offset - st->prev_offset - 1;

    if (length > LEXER_ID_MAX_LENGTH) {
        lex_error (st, "identifier is too long (%d). maximum length is %d.", length, LEXER_ID_MAX_LENGTH);
        return false;
    }

    for (int i = 0; i < length; i++) {
        ch = lex_at (st, st->prev_offset + i + 1);

        if (!(lex_isalnum (ch) || ch == '_')) {
            lex_error (st, "invalid syntax");
            return false;
        }
    }

    return true;
}


static bool
lex_check_float (lexer_state_t *st) {
    clv_debug ("%s: not implemented", __func__);
    return true;
}


static bool
lex_check_bin (lexer_state_t *st) {
    if (clv_source_compare (st->src, "0b", st->prev_offset, 2) != 0) {
        return false;
    }

    int length = st->offset - st->prev_offset - 2;

    for (int i = 0; i < length; i++) {
        char ch = lex_at (st, st->prev_offset + i + 2);

        if (!(ch == '0' || ch == '1')) {
            lex_error (st, "invalid syntax");
            return false;
        }
    }

    return true;
}


static bool
lex_check_hex (lexer_state_t *st) {
    if (clv_source_compare (st->src, "0x", st->prev_offset, 2) != 0) {
        return false;
    }

    int length = st->offset - st->prev_offset - 2;

    for (int i = 0; i < length; i++) {
        char ch = lex_at (st, st->prev_offset + i + 2);

        if (!lex_isxdigit (ch)) {
            lex_error (st, "invalid syntax");
            return false;
        }
    }

    return true;
}


static bool
lex_check_int (lexer_state_t *st) {
    int length = st->offset - st->prev_offset;

    for (int i = 0; i < length; i++) {
        char ch = lex_at (st, st->prev_offset + i);

        if (!lex_isdigit (ch)) {
            lex_error (st, "invalid syntax");
            return false;
        }
    }

    return true;
}


/* == Find Functions == */


static int
find_comment (lexer_state_t *st, clv_token_t *out_token) {
    if (!lex_arity (st, 2)) {
        return LEXER_EOF;
    }

    if (!lex_equal (st, "//", 2)) {
        return LEXER_NOT_FOUND;
    }

    st->offset += strcspn (lex_offset (st, st->offset), "\n");

    lex_commit (st, out_token, CLV_TOKEN_COMMENT);

    return LEXER_FOUND;
}


static int
find_string (lexer_state_t *st, clv_token_t *out_token) {
    if (!lex_arity (st, 1)) {
        return LEXER_EOF;
    }

    if (lex_at (st, st->offset) != '"') {
        return LEXER_NOT_FOUND;
    }

    st->offset += 1;

    while (lex_at (st, st->offset) != '"') {
        if (!lex_arity (st, 1)) {
            lex_error (st, "unclosed string literal");
            return LEXER_EOF;
        }

        if (lex_at (st, st->offset) == '\\') {
            st->offset += 1;

            if (!lex_check_escape (st)) {
                return LEXER_ERROR;
            }
        } else {
            st->offset += 1;
        }
    }

    st->offset += 1;

    lex_commit (st, out_token, CLV_TOKEN_COMMENT);

    return LEXER_FOUND;
}


static int
find_character (lexer_state_t *st, clv_token_t *out_token) {
    if (!lex_arity (st, 1)) {
        return LEXER_EOF;
    }

    if (lex_at (st, st->offset) != '\'') {
        return LEXER_NOT_FOUND;
    }

    st->offset += 1;

    int num_chars = 0;

    while (lex_at (st, st->offset) != '\'') {
        if (!lex_arity (st, 1)) {
            lex_error (st, "unclosed character literal");
            return LEXER_EOF;
        }

        if (lex_at (st, st->offset) == '\\') {
            st->offset += 1;

            if (!lex_check_escape (st)) {
                return LEXER_ERROR;
            }
        } else {
            st->offset += 1;
        }

        num_chars++;
    }

    st->offset += 1;

    if (num_chars > 1) {
        lex_error (st, "multiple characters in character literal");
        return LEXER_ERROR;
    }

    lex_commit (st, out_token, CLV_TOKEN_CHARACTER);

    return LEXER_FOUND;
}


static int
find_operator (lexer_state_t *st, clv_token_t *out_token) {
    static const lexer_pair_t operators[] = {
        { "&",  1, CLV_TOKEN_BIT_AND   },
        { "|",  1, CLV_TOKEN_BIT_OR    },
        { "^",  1, CLV_TOKEN_BIT_XOR   },
        { "<<", 2, CLV_TOKEN_BIT_SHL   },
        { ">>", 2, CLV_TOKEN_BIT_SHR   },
        { "!",  1, CLV_TOKEN_NOT       },
        { "&&", 2, CLV_TOKEN_AND       },
        { "||", 2, CLV_TOKEN_OR        },
        { "==", 2, CLV_TOKEN_EQ        },
        { "!=", 2, CLV_TOKEN_NE        },
        { "<",  1, CLV_TOKEN_LT        },
        { ">",  1, CLV_TOKEN_GT        },
        { "<=", 2, CLV_TOKEN_LE        },
        { ">=", 2, CLV_TOKEN_GE        },
        { "=",  1, CLV_TOKEN_ASSIGN    },
        { "+",  1, CLV_TOKEN_PLUS      },
        { "-",  1, CLV_TOKEN_MINUS     },
        { "*",  1, CLV_TOKEN_MULTIPLY  },
        { "/",  1, CLV_TOKEN_DIVIDE    },
        { "%",  1, CLV_TOKEN_REMAINDER },
        { ".",  1, CLV_TOKEN_PERIOD    }
    };

    if (!lex_arity (st, 1)) {
        return LEXER_EOF;
    }

    for (int i = 0; i < CLV_LENGTH (operators); i++) {
        lexer_pair_t op = operators[i];

        if (!lex_arity (st, op.length)) {
            continue;
        }

        if (!lex_equal (st, op.value, op.length)) {
            continue;
        }

        st->offset += op.length;

        lex_commit (st, out_token, op.type);

        return LEXER_FOUND;
    }

    return LEXER_NOT_FOUND;
}


static int
find_symbol (lexer_state_t *st, clv_token_t *out_token) {
    static const lexer_pair_t symbols[] = {
        { ",", 1, CLV_TOKEN_COMMA        },
        { ":", 1, CLV_TOKEN_COLON        },
        { ";", 1, CLV_TOKEN_SEMICOLON    },
        { "?", 1, CLV_TOKEN_QUESTIONMARK },
        { "(", 1, CLV_TOKEN_LPARENTHESIS },
        { ")", 1, CLV_TOKEN_RPARENTHESIS },
        { "[", 1, CLV_TOKEN_LBRACKET     },
        { "]", 1, CLV_TOKEN_RBRACKET     },
        { "{", 1, CLV_TOKEN_LBRACE       },
        { "}", 1, CLV_TOKEN_RBRACE       }
    };

    if (!lex_arity (st, 1)) {
        return LEXER_EOF;
    }

    for (int i = 0; i < CLV_LENGTH (symbols); i++) {
        lexer_pair_t sym = symbols[i];

        if (lex_at (st, st->offset) != sym.value[0]) {
            continue;
        }

        st->offset++;

        lex_commit (st, out_token, sym.type);

        return LEXER_FOUND;
    }

    return LEXER_NOT_FOUND;
}


static int
find_keyword (lexer_state_t *st, clv_token_t *out_token) {
    static const struct {
        clv_str value;
        clv_tktype_t type;
    } keywords[] = {
        { "import",   CLV_TOKEN_IMPORT   },
        { "fn",       CLV_TOKEN_FN       },
        { "type",     CLV_TOKEN_TYPE     },
        { "trait",    CLV_TOKEN_TRAIT    },
        { "defer",    CLV_TOKEN_DEFER    },
        { "struct",   CLV_TOKEN_STRUCT   },
        { "enum",     CLV_TOKEN_ENUM     },
        { "in",       CLV_TOKEN_IN       },
        { "as",       CLV_TOKEN_AS       },
        { "typeof",   CLV_TOKEN_TYPEOF   },
        { "if",       CLV_TOKEN_IF       },
        { "elif",     CLV_TOKEN_ELIF     },
        { "else",     CLV_TOKEN_ELSE     },
        { "for",      CLV_TOKEN_FOR      },
        { "while",    CLV_TOKEN_WHILE    },
        { "continue", CLV_TOKEN_CONTINUE },
        { "break",    CLV_TOKEN_BREAK    },
        { "match",    CLV_TOKEN_MATCH    },
        { "return",   CLV_TOKEN_RETURN   },
        { "let",      CLV_TOKEN_LET      },
        { "try",      CLV_TOKEN_TRY      },
        { "nil",      CLV_TOKEN_NIL      },
        { "true",     CLV_TOKEN_TRUE     },
        { "false",    CLV_TOKEN_FALSE    },
        { "pub",      CLV_TOKEN_PUB      },
        { "static",   CLV_TOKEN_STATIC   },
        { "const",    CLV_TOKEN_CONST    },
    };

    if (!lex_arity (st, 1)) {
        return LEXER_EOF;
    }

    int length = strcspn (lex_offset (st, st->offset), LEXER_DELIMITERS);

    for (int i = 0; i < CLV_LENGTH (keywords); i++) {
        if (!lex_arity (st, length)) {
            continue;
        }

        if (lex_equal (st, keywords[i].value, length)) {
            st->offset += length;
            lex_commit (st, out_token, keywords[i].type);
            return LEXER_FOUND;
        }
    }

    return LEXER_NOT_FOUND;
}


static int
find_identifier (lexer_state_t *st, clv_token_t *out_token) {
    if (!lex_arity (st, 1)) {
        return LEXER_EOF;
    }

    char first_ch = lex_at (st, st->offset);

    if (!(lex_isalpha (first_ch) || first_ch == '_')) {
        return LEXER_NOT_FOUND;
    }

    int length = strcspn (lex_offset (st, st->offset), LEXER_DELIMITERS);
    st->offset += length;

    if (!lex_check_identifier (st)) {
        return LEXER_ERROR;
    }

    lex_commit (st, out_token, CLV_TOKEN_IDENTIFIER);

    return LEXER_FOUND;
}


static int
find_float (lexer_state_t *st, clv_token_t *out_token) {
    if (!lex_arity (st, 1)) {
        return LEXER_EOF;
    }

    if (!lex_isdigit (lex_at (st, st->offset))) {
        return LEXER_NOT_FOUND;
    }

    lex_save (st);

    int int_len = strspn (lex_offset (st, st->offset), LEXER_DIGITS);
    st->offset += int_len;

    if (lex_at (st, st->offset) != '.') {
        lex_restore (st);
        return LEXER_NOT_FOUND;
    }

    st->offset += 1;

    int frac_len = strspn (lex_offset (st, st->offset), LEXER_DIGITS);
    st->offset += frac_len;

    if (!lex_check_float (st)) {
        return LEXER_ERROR;
    }

    lex_commit (st, out_token, CLV_TOKEN_FLOAT);

    return LEXER_FOUND;
}


static int
find_bin (lexer_state_t *st, clv_token_t *out_token) {
    if (!lex_arity (st, 2)) {
        return LEXER_EOF;
    }

    if (!lex_equal (st, "0b", 2)) {
        return LEXER_NOT_FOUND;
    }

    int length = strcspn (lex_offset (st, st->offset), LEXER_DELIMITERS);

    st->offset += length;

    if (!lex_check_bin (st)) {
        return LEXER_ERROR;
    }

    lex_commit (st, out_token, CLV_TOKEN_BIN);

    return LEXER_FOUND;
}


static int
find_hex (lexer_state_t *st, clv_token_t *out_token) {
    if (!lex_equal (st, "0x", 2)) {
        return LEXER_NOT_FOUND;
    }

    int length = strcspn (lex_offset (st, st->offset), LEXER_DELIMITERS);

    st->offset += length;

    if (!lex_check_hex (st)) {
        return LEXER_ERROR;
    }

    lex_commit (st, out_token, CLV_TOKEN_HEX);

    return LEXER_FOUND;
}


static int
find_int (lexer_state_t *st, clv_token_t *out_token) {
    if (!lex_arity (st, 1)) {
        return LEXER_EOF;
    }

    if (!lex_isdigit (lex_at (st, st->offset))) {
        return LEXER_NOT_FOUND;
    }

    int length = strcspn (lex_offset (st, st->offset), LEXER_DELIMITERS);

    st->offset += length;

    if (!lex_check_int (st)) {
        return LEXER_ERROR;
    }

    lex_commit (st, out_token, CLV_TOKEN_INT);

    return LEXER_FOUND;
}


static clv_token_t *
find_token (lexer_state_t *st) {
    static const lexer_func_t find_fns[] = {
        find_comment,
        find_string,
        find_character,
        find_operator,
        find_symbol,
        find_keyword,
        find_identifier,
        find_float,
        find_bin,
        find_hex,
        find_int,
    };

    lex_skip_blank (st);

    if (!lex_arity (st, 1)) {
        return NULL;
    }

    clv_token_t *token = static_cast<clv_token_t *>(malloc (sizeof (*token)));

    if (token == NULL) {
        clv_error ("failed to store token: %s", strerror (errno));
        return NULL;
    }

    int status;

    for (int i = 0; i < CLV_LENGTH (find_fns); i++) {
        status = find_fns[i] (st, token);

        if (status == LEXER_FOUND) {
            break;
        } else if (status == LEXER_EOF) {
            return NULL;
        } else if (status == LEXER_ERROR) {
            st->error = true;
            return NULL;
        }
    }

    if (status == LEXER_NOT_FOUND) {
        clv_error ("Invalid token!");
        return NULL;
    }

    return token;
}


bool
clv_lex (clv_source_t *src, clv_list_t **out_tokens) {
    clv_list_t *tokens = clv_list_new ();

    if (tokens == NULL) {
        clv_error ("%s: unable to create list: %s", strerror (errno));
        return false;
    }

    lexer_state_t st = { src, 0, 0, 0, 1, 1 };

    clv_token_t *tk;

    while ((tk = find_token (&st)) != NULL) {
        clv_list_push_back (tokens, tk);
    }

    if (clv_list_length (tokens) == 0) {
        clv_error ("file is empty: %s", clv_source_get_file (src));
        clv_list_free (tokens, NULL);
        return false;
    }

    *out_tokens = tokens;

    return !st.error;
}
