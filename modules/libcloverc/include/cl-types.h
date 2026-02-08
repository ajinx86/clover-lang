#ifndef CL_TYPES_H_
#define CL_TYPES_H_

#include "cl-core.h"


CL_ENUM(TokenType) {
    TK_COMMENT,         /* // */
    TK_ID,              /* self */
    TK_STRING,          /* "string" */
    TK_CHAR,            /* 'a' */
    TK_FLOAT,           /* 3.1415 */
    TK_BIN,             /* 0b10101010 */
    TK_HEX,             /* 0x7f7f7f7f */
    TK_INT,             /* 42 */
    KW_IMPORT,          /* import */
    KW_FN,              /* fn */
    KW_STRUCT,          /* struct */
    KW_ENUM,            /* enum */
    KW_VAR,             /* var */
    KW_CONST,           /* const */
    KW_PUB,             /* pub */
    KW_STATIC,          /* static */
    KW_AS,              /* as */
    KW_IF,              /* if */
    KW_ELSE,            /* else */
    KW_FOR,             /* for */
    KW_WHILE,           /* while */
    KW_CONTINUE,        /* continue */
    KW_BREAK,           /* break */
    KW_SWITCH,          /* switch */
    KW_DEFER,           /* defer */
    KW_RETURN,          /* return */
    KW_TRUE,            /* true */
    KW_FALSE,           /* false */
    OP_BIT_NOT,         /* ~ */
    OP_BIT_AND,         /* & */
    OP_BIT_OR,          /* | */
    OP_BIT_XOR,         /* ^ */
    OP_BIT_SHL,         /* << */
    OP_BIT_SHR,         /* >> */
    OP_NOT,             /* ! */
    OP_AND,             /* && */
    OP_OR,              /* || */
    OP_EQ,              /* == */
    OP_NE,              /* != */
    OP_LT,              /* < */
    OP_GT,              /* > */
    OP_LE,              /* <= */
    OP_GE,              /* >= */
    OP_ASSIGN,          /* = */
    OP_PLUS,            /* + */
    OP_MINUS,           /* - */
    OP_MULTIPLY,        /* * */
    OP_DIVIDE,          /* / */
    OP_REMAINDER,       /* % */
    SYM_PERIOD,         /* . */
    SYM_COMMA,          /* , */
    SYM_COLON,          /* : */
    SYM_SEMICOLON,      /* ; */
    SYM_QUESTIONMARK,   /* ? */
    SYM_LPARENTHESIS,   /* ( */
    SYM_RPARENTHESIS,   /* ) */
    SYM_LBRACKET,       /* [ */
    SYM_RBRACKET,       /* ] */
    SYM_LBRACE,         /* { */
    SYM_RBRACE,         /* } */
};


CL_TYPE(Token) {
    TokenType type;

    uint32_t offset;
    uint32_t line_offset;
    uint32_t length;

    uint32_t line;
    uint32_t column;
};

#endif /* CL_TYPES_H_ */
