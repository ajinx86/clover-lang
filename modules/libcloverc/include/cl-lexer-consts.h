#ifndef CL_LEXER_CONSTS_H_
#define CL_LEXER_CONSTS_H_

#include "cl-core.h"
#include "cl-types.h"


#define CL_ESCAPE_CHARS "abefnrt\"\'\\"
#define CL_ESCAPE_PAIRS "\a\b\e\f\n\r\t\'\\"
#define CL_DELIMITERS   " .,:;()[]{}<>^'\"|/!?&%*-+=\r\n"
#define CL_DIGITS       "0123456789"
#define CL_ID_MAXLEN    63

#define __CL_PAIR(s,k)  (LexerPair){(k),(s),(sizeof(s)-1)}


CL_TYPE(LexerPair) {
    TokenType type;
    str_t     name;
    size_t    name_length;
};


const LexerPair PRIMITIVES[] = {
    __CL_PAIR("comment",    TK_COMMENT),
    __CL_PAIR("identifier", TK_ID),
    __CL_PAIR("string",     TK_STRING),
    __CL_PAIR("char",       TK_CHAR),
    __CL_PAIR("float",      TK_FLOAT),
    __CL_PAIR("bin",        TK_BIN),
    __CL_PAIR("hex",        TK_HEX),
    __CL_PAIR("int",        TK_INT),
};


const LexerPair KEYWORDS[] = {
    __CL_PAIR("import",   KW_IMPORT),
    __CL_PAIR("fn",       KW_FN),
    __CL_PAIR("struct",   KW_STRUCT),
    __CL_PAIR("enum",     KW_ENUM),
    __CL_PAIR("var",      KW_VAR),
    __CL_PAIR("const",    KW_CONST),
    __CL_PAIR("pub",      KW_PUB),
    __CL_PAIR("static",   KW_STATIC),
    __CL_PAIR("as",       KW_AS),
    __CL_PAIR("if",       KW_IF),
    __CL_PAIR("else",     KW_ELSE),
    __CL_PAIR("for",      KW_FOR),
    __CL_PAIR("while",    KW_WHILE),
    __CL_PAIR("continue", KW_CONTINUE),
    __CL_PAIR("break",    KW_BREAK),
    __CL_PAIR("switch",   KW_SWITCH),
    __CL_PAIR("defer",    KW_DEFER),
    __CL_PAIR("return",   KW_RETURN),
    __CL_PAIR("true",     KW_TRUE),
    __CL_PAIR("false",    KW_FALSE),
};


const LexerPair OPERATORS[] = {
    __CL_PAIR("~",  OP_BIT_NOT),
    __CL_PAIR("&",  OP_BIT_AND),
    __CL_PAIR("|",  OP_BIT_OR),
    __CL_PAIR("^",  OP_BIT_XOR),
    __CL_PAIR("<<", OP_BIT_SHL),
    __CL_PAIR(">>", OP_BIT_SHR),
    __CL_PAIR("!",  OP_NOT),
    __CL_PAIR("&&", OP_AND),
    __CL_PAIR("||", OP_OR),
    __CL_PAIR("==", OP_EQ),
    __CL_PAIR("!=", OP_NE),
    __CL_PAIR("<",  OP_LT),
    __CL_PAIR(">",  OP_GT),
    __CL_PAIR("<=", OP_LE),
    __CL_PAIR(">=", OP_GE),
    __CL_PAIR("=",  OP_ASSIGN),
    __CL_PAIR("+",  OP_PLUS),
    __CL_PAIR("-",  OP_MINUS),
    __CL_PAIR("*",  OP_MULTIPLY),
    __CL_PAIR("/",  OP_DIVIDE),
    __CL_PAIR("%",  OP_REMAINDER)
};


const LexerPair SYMBOLS[] = {
    __CL_PAIR(".", SYM_PERIOD),
    __CL_PAIR(",", SYM_COMMA),
    __CL_PAIR(":", SYM_COLON),
    __CL_PAIR(";", SYM_SEMICOLON),
    __CL_PAIR("?", SYM_QUESTIONMARK),
    __CL_PAIR("(", SYM_LPARENTHESIS),
    __CL_PAIR(")", SYM_RPARENTHESIS),
    __CL_PAIR("[", SYM_LBRACKET),
    __CL_PAIR("]", SYM_RBRACKET),
    __CL_PAIR("{", SYM_LBRACE),
    __CL_PAIR("}", SYM_RBRACE)
};

#endif /* CL_LEXER_CONSTS_H_ */
