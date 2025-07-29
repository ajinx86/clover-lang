#include <lexer/lexer.hpp>
#include <lexer/lexer_priv.hpp>
#include <lexer/lexer_consts.hpp>
#include <lexer/lexer_checks.hpp>
#include <base/compiler_priv.hpp>
#include <base/diagnostic.hpp>


#include <array>
#include <cstdint>
#include <optional>
#include <functional>
#include <algorithm>


using std::operator""s;


static bool check_escape(clover::LexerState& st) {
    char ch = st.src[st.offset];

    bool found = true;

    if (clover::escape_chars.find(ch) != std::string::npos) {
        st.offset += 1;
    } else if (ch == 'x') { /* \xHH */
        st.offset += 1;

        if (!st.has_arity(2)) {
            clover::diagnostic::error("malformed escape sequence", st.loc());
            return false;
        }

        for (int i = 0; i < 2; i++) {
            ch = st.src[st.offset + i];

            if (!checks::isxdigit(ch)) {
                found = false;
                break;
            }
        }

        st.offset += 2;
    } else if (ch == 'u') { /* \uHHHH */
        st.offset += 1;

        if (!st.has_arity(4)) {
            clover::diagnostic::error("malformed escape sequence", st.loc());
            return false;
        }

        for (int i = 0; i < 4; i++) {
            ch = st.src[st.offset + i];

            if (!checks::isxdigit(ch)) {
                found = false;
                break;
            }
        }

        st.offset += 4;
    } else if (ch == 'U') { /* \UHHHHHHHH */
        st.offset += 1;

        if (!st.has_arity(8)) {
            clover::diagnostic::error("malformed escape sequence", st.loc());
            return false;
        }

        for (int i = 0; i < 8; i++) {
            ch = st.src[st.offset + i];

            if (!checks::isxdigit(ch)) {
                found = false;
                break;
            }
        }

        st.offset += 8;
    } else {
        found = false;
    }

    if (!found) {
        clover::diagnostic::error("invalid escape sequence", st.loc());
    }

    return found;
}


static bool check_identifier(clover::LexerState& st) {
    if (!checks::isname(st.src[st.prev_offset])) {
        return false;
    }

    uint32_t length = st.offset - st.prev_offset - 1;

    if (length > clover::max_id_length) {
        clover::diagnostic::error("identifier is too long: "s + std::to_string(length), st.loc());
        clover::diagnostic::note("maximum identifier length is "s + std::to_string(length), st.loc());
        return false;
    }

    for (uint32_t i = 0; i < length; i++) {
        if ((i == 0 && !checks::isname(st.src[st.prev_offset + i])) ||
            (i >= 1 && !checks::isalnum(st.src[st.prev_offset + i]))) {
            clover::diagnostic::error("invalid syntax", st.loc());
            return false;
        }
    }

    return true;
}


static bool check_float(clover::LexerState& st) {
    std::cerr << __PRETTY_FUNCTION__ << ": not implemented";
    return true;
}


static bool check_bin(clover::LexerState& st) {
    if (!st.equals("0b")) {
        return false;
    }

    int length = st.offset - st.prev_offset - 2;

    for (int i = 0; i < length; i++) {
        char ch = st.src[st.prev_offset + i + 2];

        if (ch != '0' && ch != '1') {
            clover::diagnostic::error("invalid syntax", st.loc());
            return false;
        }
    }

    return true;
}


static bool check_hex(clover::LexerState& st) {
    if (!st.equals("0x")) {
        return false;
    }

    int length = st.offset - st.prev_offset - 2;

    for (int i = 0; i < length; i++) {
        char ch = st.src[st.prev_offset + i + 2];

        if (!checks::isxdigit(ch)) {
            clover::diagnostic::error("invalid syntax", st.loc());
            return false;
        }
    }

    return true;
}


static bool check_int(clover::LexerState& st) {
    int length = st.offset - st.prev_offset;

    for (int i = 0; i < length; i++) {
        char ch = st.src[st.prev_offset + i];

        if (!checks::isdigit(ch)) {
            clover::diagnostic::error ("invalid syntax", st.loc());
            return false;
        }
    }

    return true;
}


// == find functions ==


static clover::LexerResult find_comment(clover::LexerState& st) {
    if (!st.has_arity(2)) {
        return clover::LexerResult::not_found;
    }

    if (!st.equals("//")) {
        return clover::LexerResult::not_found;
    }

    st.offset += st.src.nspan(st.offset, "\n");

    return st.commit(clover::TokenType::tk_comment);
}


static clover::LexerResult find_string(clover::LexerState& st) {
    if (st.eof()) {
        return clover::LexerResult::not_found;
    }

    if (st.src[st.offset] != '"') {
        return clover::LexerResult::not_found;
    }

    st.offset += 1;

    while (st.src[st.offset] != '"') {
        if (st.eof()) {
            clover::diagnostic::error("unclosed string literal", st.loc());
            return clover::LexerResult::end_of_file;
        }

        if (st.src[st.offset] == '\\') {
            st.offset += 1;

            if (!check_escape(st)) {
                clover::diagnostic::error("invalid escape sequence", st.loc());
                return clover::LexerResult::syntax_error;
            }
        } else {
            st.offset += 1;
        }
    }

    st.offset += 1;

    return st.commit(clover::TokenType::tk_string);
}


static clover::LexerResult find_character(clover::LexerState& st) {
    if (st.eof()) {
        return clover::LexerResult::not_found;
    }

    if (st.src[st.offset] != '\'') {
        return clover::LexerResult::not_found;
    }

    st.offset += 1;

    while (st.src[st.offset] != '\'') {
        if (st.eof()) {
            clover::diagnostic::error("unclosed character literal", st.loc());
            return clover::LexerResult::end_of_file;
        }

        if (st.src[st.offset] == '\\') {
            st.offset += 1;

            if (!check_escape(st)) {
                return clover::LexerResult::syntax_error;
            }
        } else {
            st.offset += 1;
        }
    }

    st.offset += 1;

    return st.commit(clover::TokenType::tk_string);
}


static clover::LexerResult find_operator(clover::LexerState& st) {
    if (st.eof()) {
        return clover::LexerResult::end_of_file;
    }

    for (auto& op : clover::operators) {
        if (!st.has_arity(op.second.length())) {
            continue;
        }

        if (!st.equals(op.second)) {
            continue;
        }

        st.offset += op.second.length();

        return st.commit(op.first);
    }

    return clover::LexerResult::not_found;
}


static clover::LexerResult find_symbol(clover::LexerState& st) {
    if (st.eof()) {
        return clover::LexerResult::end_of_file;
    }

    for (auto& sym : clover::symbols) {
        if (!st.equals(sym.second)) {
            continue;
        }

        st.offset++;

        return st.commit(sym.first);
    }

    return clover::LexerResult::not_found;
}


static clover::LexerResult find_keyword(clover::LexerState& st) {
    if (st.eof()) {
        return clover::LexerResult::end_of_file;
    }

    for (auto& kw : clover::keywords) {
        if (!st.has_arity(kw.second.length())) {
            continue;
        }

        if (st.equals(kw.second)) {
            st.offset += kw.second.length();
            return st.commit(kw.first);
        }
    }

    return clover::LexerResult::not_found;
}


static clover::LexerResult find_identifier(clover::LexerState& st) {
    if (st.eof()) {
        return clover::LexerResult::end_of_file;
    }

    if (!checks::isname(st.src[st.offset])) {
        return clover::LexerResult::not_found;
    }

    st.offset += st.src.nspan(st.offset, clover::delimiters);

    if (!check_identifier(st)) {
        return clover::LexerResult::syntax_error;
    }

    return st.commit(clover::TokenType::tk_identifier);
}


static clover::LexerResult find_float(clover::LexerState& st) {
    if (st.eof()) {
        return clover::LexerResult::end_of_file;
    }

    if (!checks::isdigit(st.src[st.offset])) {
        return clover::LexerResult::not_found;
    }

    st.save_state();

    st.offset += st.src.nspan(st.offset, clover::digits);

    if (st.src[st.offset] != '.') {
        st.restore_state();
        return clover::LexerResult::not_found;
    }

    st.offset++;

    st.offset += st.src.nspan(st.offset, clover::digits);

    if (!check_float(st)) {
        return clover::LexerResult::syntax_error;
    }

    return st.commit(clover::TokenType::tk_float);
}


static clover::LexerResult find_bin(clover::LexerState& st) {
    if (!st.equals("0b")) {
        return clover::LexerResult::not_found;
    }

    st.offset += st.src.nspan(st.offset, clover::delimiters);

    if (!check_bin(st)) {
        return clover::LexerResult::syntax_error;
    }

    return st.commit(clover::TokenType::tk_bin);
}


static clover::LexerResult find_hex(clover::LexerState& st) {
    if (!st.equals("0x")) {
        return clover::LexerResult::not_found;
    }

    st.offset += st.src.nspan(st.offset, clover::delimiters);

    if (!check_hex(st)) {
        return clover::LexerResult::syntax_error;
    }

    return st.commit(clover::TokenType::tk_hex);
}


static clover::LexerResult find_int(clover::LexerState& st) {
    if (!checks::isdigit(st.src[st.offset])) {
        return clover::LexerResult::not_found;
    }

    st.offset += st.src.nspan(st.offset, clover::delimiters);

    if (!check_int(st)) {
        return clover::LexerResult::syntax_error;
    }

    return st.commit(clover::TokenType::tk_int);
}


static clover::LexerResult __fallback(clover::LexerState& st) {
    clover::diagnostic::error("unexpected token", st.loc());
    return clover::LexerResult::syntax_error;
}


// == lexer ==


static std::optional<clover::Token> find_token(clover::LexerState& st) {
    static const std::array<clover::LexerFunc, 12> find_funcs = {
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
        __fallback,
    };

    st.skip_blank();

    if (st.eof()) {
        return std::nullopt;
    }

    for (auto& _find : find_funcs) {
        clover::LexerResult result = _find(st);

        if (result.status == clover::LexerResult::good) {
            return result.token;
        } else if (result.status == clover::LexerResult::not_found) {
            continue;
        } else if (result.status == clover::LexerResult::end_of_file) {
            return std::nullopt;
        } else if (result.status == clover::LexerResult::syntax_error) {
            st.error = true;
            return std::nullopt;
        }
    }

    return std::nullopt;
}


bool clover::lex(CompileUnit& unit) {
    clover::LexerState st { unit.src };

    while (auto token = find_token(st)) {
        if (token.value().type == TokenType::tk_comment) {
            continue; // skipping comments
        }

        unit.tokens.push_back(token.value());
    }

    return !st.error;
}
