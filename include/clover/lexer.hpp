#ifndef CLOVER_LEXER_H_
#define CLOVER_LEXER_H_

#include <clover/base.hpp>
#include <clover/source.hpp>
#include <clover/list.hpp>


typedef enum {
    CLV_TOKEN_COMMENT,      // //
    CLV_TOKEN_IDENTIFIER,   // self
    CLV_TOKEN_STRING,       // "string"
    CLV_TOKEN_CHARACTER,    // 'a'
    CLV_TOKEN_FLOAT,        // 3.1415926536
    CLV_TOKEN_INT,          // 10
    CLV_TOKEN_BIN,          // 0b10101010
    CLV_TOKEN_HEX,          // 0x7ffffdb0
    CLV_TOKEN_IMPORT,       // import
    CLV_TOKEN_FN,           // fn
    CLV_TOKEN_TYPE,         // type
    CLV_TOKEN_TRAIT,        // trait
    CLV_TOKEN_DEFER,        // defer
    CLV_TOKEN_STRUCT,       // struct
    CLV_TOKEN_ENUM,         // enum
    CLV_TOKEN_IN,           // in
    CLV_TOKEN_AS,           // as
    CLV_TOKEN_TYPEOF,       // typeof
    CLV_TOKEN_IF,           // if
    CLV_TOKEN_ELIF,         // elif
    CLV_TOKEN_ELSE,         // else
    CLV_TOKEN_FOR,          // for
    CLV_TOKEN_WHILE,        // while
    CLV_TOKEN_CONTINUE,     // continue
    CLV_TOKEN_BREAK,        // break
    CLV_TOKEN_MATCH,        // match
    CLV_TOKEN_RETURN,       // return
    CLV_TOKEN_LET,          // let
    CLV_TOKEN_TRY,          // try
    CLV_TOKEN_NIL,          // nil
    CLV_TOKEN_TRUE,         // true
    CLV_TOKEN_FALSE,        // false
    CLV_TOKEN_PUB,          // pub
    CLV_TOKEN_STATIC,       // static
    CLV_TOKEN_CONST,        // const
    CLV_TOKEN_BIT_NOT,      // ~
    CLV_TOKEN_BIT_AND,      // &
    CLV_TOKEN_BIT_OR,       // |
    CLV_TOKEN_BIT_XOR,      // ^
    CLV_TOKEN_BIT_SHL,      // <<
    CLV_TOKEN_BIT_SHR,      // >>
    CLV_TOKEN_NOT,          // !
    CLV_TOKEN_AND,          // &&
    CLV_TOKEN_OR,           // ||
    CLV_TOKEN_EQ,           // ==
    CLV_TOKEN_NE,           // !=
    CLV_TOKEN_LT,           // <
    CLV_TOKEN_GT,           // >
    CLV_TOKEN_LE,           // <=
    CLV_TOKEN_GE,           // >=
    CLV_TOKEN_ASSIGN,       // =
    CLV_TOKEN_PLUS,         // +
    CLV_TOKEN_MINUS,        // -
    CLV_TOKEN_MULTIPLY,     // *
    CLV_TOKEN_DIVIDE,       // /
    CLV_TOKEN_REMAINDER,    // %
    CLV_TOKEN_PERIOD,       // .
    CLV_TOKEN_COMMA,        // ,
    CLV_TOKEN_COLON,        // :
    CLV_TOKEN_SEMICOLON,    // ;
    CLV_TOKEN_QUESTIONMARK, // ?
    CLV_TOKEN_LPARENTHESIS, // (
    CLV_TOKEN_RPARENTHESIS, // )
    CLV_TOKEN_LBRACKET,     // [
    CLV_TOKEN_RBRACKET,     // ]
    CLV_TOKEN_LBRACE,       // {
    CLV_TOKEN_RBRACE,       // }
} clv_tktype_t;


typedef struct {
    clv_tktype_t type;

    uint32_t offset;
    uint32_t line_offset;
    uint32_t length;

    uint32_t line;
    uint32_t column;
} clv_token_t;


bool clv_lex (clv_source_t *src, clv_list_t **out_tokens);

#endif /* CLOVER_LEXER_H_ */
