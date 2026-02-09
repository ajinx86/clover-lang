#include "cl-annotation.h"

#include "cl-log.h"
#include "cl-types.h"
#include "cl-vector.h"
#include "cl-diagnostic.h"
#include "cl-lexer-consts.h"


CL_TYPE(Lexer) {
    Source *src;

    uint32_t offset;
    uint32_t prev_offset;
    uint32_t line_offset;

    uint32_t line;
    uint32_t column;

    bool error;
};


CL_ENUM(LexerRet) {
    LEXER_OK,
    LEXER_EOF,
    LEXER_NOT_FOUND,
    LEXER_SYNTAX_ERROR,
};


typedef LexerRet (*LexerFn)(Lexer *lex, Token *tk);


/* == lexer checks == */


static __Inline bool __isalnum(char ch) {
    return ((ch >= '0' && ch <= '9') ||
            (ch >= 'a' && ch <= 'z') ||
            (ch >= 'A' && ch <= 'Z'));
}


static __Inline bool __isname(char ch) {
    return ((ch >= 'a' && ch <= 'z') ||
            (ch >= 'A' && ch <= 'Z') ||
            (ch == '_'));
};


static __Inline bool __isdigit(char ch) {
    return (ch >= '0' && ch <= '9');
}


static __Inline bool __isxdigit(char ch) {
    return ((ch >= '0' && ch <= '9') ||
            (ch >= 'a' && ch <= 'f') ||
            (ch >= 'A' && ch <= 'F'));
};


static __Inline bool __isbindigit(char ch) {
    return (ch == '0' || ch == '1');
}


static __Inline bool __strcontains(str_t str, char ch) {
    for (int i = 0; str[i] != 0; i++) {
        if (str[i] == ch) {
            return true;
        }
    }

    return false;
}


/* == lexer functions == */


static __Inline bool _is_eof(Lexer *lex) {
    return lex->offset >= lex->src->length;
}


static __Inline bool _has_arity(Lexer *lex, uint32_t length) {
    return lex->offset + length <= lex->src->length;
}


static __Inline char _peek(Lexer *lex) {
    return source_at(lex->src, lex->offset);
}


static __Inline char _getch(Lexer *lex, size_t offset) {
    return source_at(lex->src, offset);
}


static __Inline bool _equals(Lexer *lex, size_t offset, str_t str,
    uint32_t str_max) {
    return source_cmp(lex->src, offset, str_max, str) == 0;
}


static __Inline DiagLocation _getloc(Lexer *lex, uint32_t caret_pos) {
    return (DiagLocation){
        .offset = lex->offset,
        .line_offset = lex->line_offset,
        .length = lex->offset - lex->prev_offset,
        .line_length = source_lnlen(lex->src, lex->line_offset),
        .line = lex->line,
        .column = lex->column,
        .caret = caret_pos,
        .src = lex->src,
    };
}


static void _skip_blank(Lexer *lex) {
    uint32_t count = 0;

    for (int left = 1; left > 0; left--) {
        if (_is_eof(lex)) {
            break;
        }

        count = source_span(lex->src, lex->offset, "\t ");

        if (count > 0) {
            lex->offset += count;
            lex->column += count;
            left++;
        }

        count = source_span(lex->src, lex->offset, "\r\n");

        if (count > 0) {
            lex->offset += count;
            lex->line_offset = lex->offset;
            lex->line += count;
            lex->column = 1;
            left++;

            continue;
        }
    }

    lex->prev_offset = lex->offset;
}


static void _commit(Lexer *lex, TokenType type, Token *tk) {
    if (tk != NULL) {
        tk->type = type;
        tk->offset = lex->prev_offset;
        tk->line_offset = lex->line_offset;
        tk->length = lex->offset - lex->prev_offset;
        tk->line = lex->line;
        tk->column = lex->column;
    }

    lex->column += lex->offset - lex->prev_offset;
    lex->prev_offset = lex->offset;
}


static bool _validate_escape(Lexer *lex) {
    char ch = _peek(lex);

    if (__strcontains(CL_ESCAPE_CHARS, ch)) {
        lex->offset += 1;
        return true;
    }

    int fmt_size = 0;

    switch (ch) {
        case 'x':
            fmt_size = 2;
            break;
        case 'u':
            fmt_size = 4;
            break;
        case 'U':
            fmt_size = 8;
            break;
        default:
            diag_error(_getloc(lex, 0), "invalid escape sequence");
            return false;
    }

    for (int i = 0; i < fmt_size; i++) {
        if (!__isxdigit(_getch(lex, lex->offset + i + 1))) {
            diag_error(_getloc(lex, 0), "malformed escape sequence");
            return false;
        }
    }

    return true;
}


static bool _validate_id(Lexer *lex) {
    if (!__isname(_getch(lex, lex->prev_offset))) {
        return false;
    }

    uint32_t length = lex->offset - lex->prev_offset - 1;

    if (length > CL_ID_MAXLEN) {
        DiagLocation loc = _getloc(lex, 0);
        diag_warning(loc, "identifier is too long: %zu", length);
        diag_note(loc, "the maximum recommended length is %zu", CL_ID_MAXLEN);
        return false;
    }

    for (uint32_t i = 1; i < length; i++) {
        if (!__isalnum(_getch(lex, lex->prev_offset + i))) {
            diag_error(_getloc(lex, 0), "invalid syntax");
            return false;
        }
    }

    return true;
}


static bool _validate_float(Lexer *lex) {
    if (!__isdigit(_getch(lex, lex->prev_offset))) {
        return false;
    }

    uint32_t length = lex->offset - lex->prev_offset;

    bool has_period = false;

    for (uint32_t i = 0; i < length; i++) {
        char ch = _getch(lex, lex->prev_offset + i);

        if (ch == '.') {
            if (!has_period) {
                has_period = true;
                continue;
            }

            diag_error(_getloc(lex, i), "invalid syntax");
            return false;
        }

        if (!__isdigit(ch)) {
            diag_error(_getloc(lex, i), "invalid syntax");
            return false;
        }
    }

    if (!__isdigit(_getch(lex, lex->prev_offset + length - 1))) {
        diag_error(_getloc(lex, length - 1), "invalid syntax");
        return false;
    }

    if (!has_period) {
        diag_error(_getloc(lex, 0),
            "floats must have the following format: 3.14, 4.0f32");
        return false;
    }

    return true;
}


static bool _validate_bin(Lexer *lex) {
    if (!_equals(lex, lex->prev_offset, "0b", 2)) {
        return false;
    }

    uint32_t length = lex->offset - lex->prev_offset - 2;

    for (uint32_t i = 2; i < length; i++) {
        if (!__isbindigit(_getch(lex, lex->prev_offset + i + 2))) {
            diag_error(_getloc(lex, 0), "invalid syntax");
            return false;
        }
    }

    return true;
}


static bool _validate_hex(Lexer *lex) {
    if (!_equals(lex, lex->prev_offset, "0x", 2)) {
        return false;
    }

    uint32_t length = lex->offset - lex->prev_offset - 2;

    for (uint32_t i = 2; i < length; i++) {
        if (!__isxdigit(_getch(lex, lex->prev_offset + i + 2))) {
            diag_error(_getloc(lex, i + 2), "invalid syntax");
            return false;
        }
    }

    return true;
}


static bool _validate_int(Lexer *lex) {
    uint32_t length = lex->offset - lex->prev_offset;

    for (uint32_t i = 0; i < length; i++) {
        if (!__isdigit(_getch(lex, lex->prev_offset + i))) {
            diag_error(_getloc(lex, i + 2), "invalid syntax");
            return false;
        }
    }

    return true;
}


/* === Search Functions === */


static LexerRet skip_comment(Lexer *lex, Token *tk) {
    if (!_equals(lex, 0, "//", 2)) {
        return LEXER_NOT_FOUND;
    }

    lex->offset += source_cspan(lex->src, lex->offset, "\r\n");

    _commit(lex, TK_COMMENT, NULL);

    return LEXER_NOT_FOUND; /* always returns NOT FOUND */
}


static LexerRet find_string(Lexer *lex, Token *tk) {
    if (_is_eof(lex)) {
        return LEXER_EOF;
    }

    if (_peek(lex) != '"') {
        return LEXER_NOT_FOUND;
    }

    lex->offset += 1;

    while (_peek(lex) != '"') {
        if (_is_eof(lex)) {
            diag_error(_getloc(lex, 0), "unclosed string literal");
            return LEXER_EOF;
        }

        char ch = _peek(lex);
        lex->offset += 1;

        if (ch == '\\' && !_validate_escape(lex)) {
            return LEXER_SYNTAX_ERROR;
        }
    }

    lex->offset += 1;

    _commit(lex, TK_STRING, tk);

    return LEXER_OK;
}


static LexerRet find_character(Lexer *lex, Token *tk) {
    if (_is_eof(lex)) {
        return LEXER_EOF;
    }

    if (_peek(lex) != '\'') {
        return LEXER_NOT_FOUND;
    }

    lex->offset += 1;

    int num_chars = 0;

    while (_peek(lex) != '\'') {
        if (_is_eof(lex)) {
            diag_error(_getloc(lex, 0), "unclosed character literal");
            return LEXER_EOF;
        }

        char ch = _peek(lex);
        lex->offset += 1;

        if (ch == '\\' && !_validate_escape(lex)) {
            return LEXER_SYNTAX_ERROR;
        }

        num_chars += 1;
    }

    lex->offset += 1;

    if (num_chars > 1) {
        diag_error(_getloc(lex, 0), "multiple characters in character literal");
        return LEXER_SYNTAX_ERROR;
    }

    _commit(lex, TK_CHAR, tk);

    return LEXER_OK;
}


static LexerRet find_operator(Lexer *lex, Token *tk) {
    if (_is_eof(lex)) {
        return LEXER_EOF;
    }

    for (size_t i = 0; i < CL_N_ELEMS(OPERATORS); i++) {
        LexerPair op = OPERATORS[i];

        if (!_has_arity(lex, op.name_length)) {
            continue;
        }

        if (_equals(lex, 0, op.name, op.name_length)) {
            lex->offset += op.name_length;
            _commit(lex, op.type, tk);

            return LEXER_OK;
        }
    }

    return LEXER_NOT_FOUND;
}


static LexerRet find_symbol(Lexer *lex, Token *tk) {
    if (_is_eof(lex)) {
        return LEXER_EOF;
    }

    for (size_t i = 0; i < CL_N_ELEMS(SYMBOLS); i++) {
        LexerPair sym = SYMBOLS[i];

        if (_peek(lex) == sym.name[0]) {
            lex->offset += 1;
            _commit(lex, sym.type, tk);

            return LEXER_OK;
        }
    }

    return LEXER_NOT_FOUND;
}


static LexerRet find_keyword(Lexer *lex, Token *tk) {
    if (_is_eof(lex)) {
        return LEXER_EOF;
    }

    for (size_t i = 0; i < CL_N_ELEMS(KEYWORDS); i++) {
        LexerPair kw = KEYWORDS[i];

        if (!_has_arity(lex, kw.name_length)) {
            continue;
        }

        if (_equals(lex, lex->offset, kw.name, kw.name_length)) {
            lex->offset += kw.name_length;
            _commit(lex, kw.type, tk);

            return LEXER_OK;
        }
    }

    return LEXER_NOT_FOUND;
}


static LexerRet find_identifier(Lexer *lex, Token *tk) {
    if (_is_eof(lex)) {
        return LEXER_EOF;
    }

    if (!__isname(_peek(lex))) {
        return LEXER_NOT_FOUND;
    }

    lex->offset += source_cspan(lex->src, lex->offset, CL_DELIMITERS);

    if (!_validate_id(lex)) {
        return LEXER_SYNTAX_ERROR;
    }

    _commit(lex, TK_ID, tk);

    return LEXER_OK;
}


static LexerRet find_float(Lexer *lex, Token *tk) {
    if (_is_eof(lex)) {
        return LEXER_EOF;
    }

    if (!__isdigit(_peek(lex))) {
        return LEXER_NOT_FOUND;
    }

    size_t saved_offset = lex->offset;

    lex->offset += source_cspan(lex->src, lex->offset, CL_DELIMITERS);

    if (_peek(lex) != '.') {
        lex->offset = saved_offset;
        return LEXER_NOT_FOUND;
    }

    lex->offset += 1;
    lex->offset += source_cspan(lex->src, lex->offset, CL_DELIMITERS);

    if (!_validate_float(lex)) {
        return LEXER_SYNTAX_ERROR;
    }

    _commit(lex, TK_FLOAT, tk);

    return LEXER_OK;
}


static LexerRet find_bin(Lexer *lex, Token *tk) {
    if (!_equals(lex, 0, "0b", 2)) {
        return LEXER_NOT_FOUND;
    }

    lex->offset += source_cspan(lex->src, lex->offset, CL_DELIMITERS);

    if (!_validate_bin(lex)) {
        return LEXER_SYNTAX_ERROR;
    }

    _commit(lex, TK_BIN, tk);

    return LEXER_OK;
}


static LexerRet find_hex(Lexer *lex, Token *tk) {
    if (!_equals(lex, 0, "0x", 2)) {
        return LEXER_NOT_FOUND;
    }

    lex->offset += source_cspan(lex->src, lex->offset, CL_DELIMITERS);

    if (!_validate_hex(lex)) {
        return LEXER_SYNTAX_ERROR;
    }

    _commit(lex, TK_HEX, tk);

    return LEXER_OK;
}


static LexerRet find_int(Lexer *lex, Token *tk) {
    if (!__isdigit(_peek(lex))) {
        return LEXER_NOT_FOUND;
    }

    lex->offset += source_cspan(lex->src, lex->offset, CL_DELIMITERS);

    if (!_validate_int(lex)) {
        return LEXER_SYNTAX_ERROR;
    }

    _commit(lex, TK_INT, tk);

    return LEXER_OK;
}


static LexerRet __fallback(Lexer *lex, Token *tk) {
    diag_error(_getloc(lex, 0), "unexpected token");
    return LEXER_SYNTAX_ERROR;
}


static bool find_token(Lexer *lex, Token *out_tk) {
    static const LexerFn find_fns[] = {
        skip_comment,
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
        __fallback
    };

    _skip_blank(lex);

    if (_is_eof(lex)) {
        return false;
    }

    Token tmp;

    for (int i = 0; i < CL_N_ELEMS(find_fns); i++) {
        LexerRet result = find_fns[i](lex, &tmp);

        switch (result) {
            case LEXER_OK:
                *out_tk = tmp;
                return true;
            case LEXER_EOF:
                return false;
            case LEXER_NOT_FOUND:
                break; /* try next */
            case LEXER_SYNTAX_ERROR:
                lex->error = true;
                return false;
            default:
                cl_debug("unexpected result: %d\n", result);
                break;
        }
    }

    return false;
}


bool cl_lex(Source *src, Vector *tokens) {
    Lexer lex = { src, 0, 0, 0, 1, 1, false };

    Token tk;

    while (!_is_eof(&lex)) {
        if (!find_token(&lex, &tk)) {
            break;
        }

        if (!vector_push(tokens, &tk)) {
            cl_error("out of memory!\n");
            return false;
        }
    }

    return !lex.error;
}
