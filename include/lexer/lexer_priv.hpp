#pragma once

#include <base/diagnostic.hpp>
#include <base/source.hpp>


namespace clover
{

    using std::operator""s;


    enum class TokenType : uint8_t
    {
        tk_comment,         // //
        tk_identifier,      // self
        tk_string,          // "string"
        tk_character,       // 'a'
        tk_float,           // 3.1415926536
        tk_int,             // 10
        tk_bin,             // 0b10101010
        tk_hex,             // 0x7ffffdb0
        kw_import,          // import
        kw_fn,              // fn
        kw_type,            // type
        kw_trait,           // trait
        kw_defer,           // defer
        kw_struct,          // struct
        kw_enum,            // enum
        kw_in,              // in
        kw_as,              // as
        kw_typeof,          // typeof
        kw_if,              // if
        kw_elif,            // elif
        kw_else,            // else
        kw_for,             // for
        kw_while,           // while
        kw_continue,        // continue
        kw_break,           // break
        kw_match,           // match
        kw_return,          // return
        kw_let,             // let
        kw_try,             // try
        kw_nil,             // nil
        kw_true,            // true
        kw_false,           // false
        kw_pub,             // pub
        kw_static,          // static
        kw_const,           // const
        op_bit_not,         // ~
        op_bit_and,         // &
        op_bit_or,          // |
        op_bit_xor,         // ^
        op_bit_shl,         // <<
        op_bit_shr,         // >>
        op_not,             // !
        op_and,             // &&
        op_or,              // ||
        op_eq,              // ==
        op_ne,              // !=
        op_lt,              // <
        op_gt,              // >
        op_le,              // <=
        op_ge,              // >=
        op_assign,          // =
        op_plus,            // +
        op_minus,           // -
        op_multiply,        // *
        op_divide,          // /
        op_remainder,       // %
        sym_period,         // .
        sym_comma,          // ,
        sym_colon,          // :
        sym_semicolon,      // ;
        sym_questionmark,   // ?
        sym_lparenthesis,   // (
        sym_rparenthesis,   // )
        sym_lbracket,       // [
        sym_rbracket,       // ]
        sym_lbrace,         // {
        sym_rbrace,         // }
    };


    struct Token
    {
        TokenType type;

        uint32_t offset;
        uint32_t line_offset;
        uint32_t length;

        uint32_t line;
        uint32_t column;

        std::string value; //! TODO: find a better way to save memory
    };


    struct LexerState {

        const Source& src;

        uint32_t offset = 0;
        uint32_t prev_offset = 0;
        uint32_t line_offset = 0;

        uint32_t line = 1;
        uint32_t column = 1;

        struct {
            uint32_t offset;
            uint32_t prev_offset;
            uint32_t line_offset;

            uint32_t line;
            uint32_t column;
        } saved;

        bool error = false;

        Token commit(TokenType type) {
            Token token = {
                .type = type,
                .offset = prev_offset,
                .line_offset = line_offset,
                .length = offset - prev_offset,
                .line = line,
                .column = column,
                .value = src.substr(prev_offset, offset - prev_offset),
            };

            column += offset - prev_offset;
            prev_offset = offset;

            return token;
        }

        void save_state() {
            saved.offset = offset;
            saved.prev_offset = prev_offset;
            saved.line_offset = line_offset;
            saved.line = line;
            saved.column = column;
        }

        void restore_state() {
            offset = saved.offset;
            prev_offset = saved.prev_offset;
            line_offset = saved.line_offset;
            line = saved.line;
            column = saved.column;
        }

        void skip_blank() {
            uint32_t count;

            for (int left = 2; left > 0; left--) {
                if (eof()) {
                    break;
                }

                count = src.span(offset, "\t ");

                if (count > 0) {
                    offset += count;
                    column += count;
                    left = 2;

                    continue;
                }

                count = src.span(offset, "\n");

                if (count > 0) {
                    offset += count;
                    line_offset = offset;
                    line += count;
                    column = 1;

                    left = 2;
                    continue;
                }
            }

            prev_offset = offset;
        }

        inline clover::diagnostic::Location loc() const {
            return {
                .offset = prev_offset,
                .line_offset = line_offset,
                .length = (offset - prev_offset),
                .line = line,
                .column = column,
                .scope = "",
                .src = src
            };
        }

        inline bool eof() const { return offset >= src.length(); }

        inline bool equals(const std::string& s) const { return src.compare(offset, s.length(), s) == 0; }

        inline bool has_arity(uint32_t length) const { return offset + length <= src.length(); }

    };


    struct LexerResult
    {
        const enum Status
        {
            good,
            not_found,
            end_of_file,
            syntax_error
        } status;

        const std::optional<Token> token = std::nullopt;

        constexpr LexerResult(Token&& _token) :
            status(good), token(_token) {}

        constexpr LexerResult(Status _status) :
            status(_status), token(std::nullopt) {}
    };


    typedef LexerResult(*LexerFunc)(LexerState& st);

}
