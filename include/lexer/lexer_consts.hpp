#pragma once

#include <lexer/lexer_priv.hpp>

#include <array>
#include <string>
#include <cstdint>


namespace clover {

    const std::string escape_chars  = "abefnrtv\"\'\\";
    const std::string escape_pairs  = "\a\b\e\f\n\r\t\v\'\\";
    const std::string delimiters    = " .,:;()[]{}<>^'\"|/!?&%*-+=\r\n";
    const std::string digits        = "0123456789";
    const uint32_t    max_id_length = 63;

    typedef std::pair<TokenType, std::string> LexerPair;

    static const std::array<LexerPair, 8> primitives = {
        LexerPair{ TokenType::tk_comment,    "comment"    },
        LexerPair{ TokenType::tk_identifier, "identifier" },
        LexerPair{ TokenType::tk_string,     "string"     },
        LexerPair{ TokenType::tk_character,  "character"  },
        LexerPair{ TokenType::tk_float,      "float"      },
        LexerPair{ TokenType::tk_int,        "integer"    },
        LexerPair{ TokenType::tk_bin,        "integer"    },
        LexerPair{ TokenType::tk_hex,        "integer"    }
    };


    static const std::array<LexerPair, 27> keywords = {
        LexerPair{ TokenType::kw_import,   "import"   },
        LexerPair{ TokenType::kw_fn,       "fn"       },
        LexerPair{ TokenType::kw_type,     "type"     },
        LexerPair{ TokenType::kw_trait,    "trait"    },
        LexerPair{ TokenType::kw_defer,    "defer"    },
        LexerPair{ TokenType::kw_struct,   "struct"   },
        LexerPair{ TokenType::kw_enum,     "enum"     },
        LexerPair{ TokenType::kw_in,       "in"       },
        LexerPair{ TokenType::kw_as,       "as"       },
        LexerPair{ TokenType::kw_typeof,   "typeof"   },
        LexerPair{ TokenType::kw_if,       "if"       },
        LexerPair{ TokenType::kw_elif,     "elif"     },
        LexerPair{ TokenType::kw_else,     "else"     },
        LexerPair{ TokenType::kw_for,      "for"      },
        LexerPair{ TokenType::kw_while,    "while"    },
        LexerPair{ TokenType::kw_continue, "continue" },
        LexerPair{ TokenType::kw_break,    "break"    },
        LexerPair{ TokenType::kw_match,    "match"    },
        LexerPair{ TokenType::kw_return,   "return"   },
        LexerPair{ TokenType::kw_let,      "let"      },
        LexerPair{ TokenType::kw_try,      "try"      },
        LexerPair{ TokenType::kw_nil,      "nil"      },
        LexerPair{ TokenType::kw_true,     "true"     },
        LexerPair{ TokenType::kw_false,    "false"    },
        LexerPair{ TokenType::kw_pub,      "pub"      },
        LexerPair{ TokenType::kw_static,   "static"   },
        LexerPair{ TokenType::kw_const,    "const"    },
    };


    static const std::array<LexerPair, 20> operators = {
        LexerPair{ TokenType::op_bit_and,   "&"  },
        LexerPair{ TokenType::op_bit_or,    "|"  },
        LexerPair{ TokenType::op_bit_xor,   "^"  },
        LexerPair{ TokenType::op_bit_shl,   "<<" },
        LexerPair{ TokenType::op_bit_shr,   ">>" },
        LexerPair{ TokenType::op_not,       "!"  },
        LexerPair{ TokenType::op_and,       "&&" },
        LexerPair{ TokenType::op_or,        "||" },
        LexerPair{ TokenType::op_eq,        "==" },
        LexerPair{ TokenType::op_ne,        "!=" },
        LexerPair{ TokenType::op_lt,        "<"  },
        LexerPair{ TokenType::op_gt,        ">"  },
        LexerPair{ TokenType::op_le,        "<=" },
        LexerPair{ TokenType::op_ge,        ">=" },
        LexerPair{ TokenType::op_assign,    "="  },
        LexerPair{ TokenType::op_plus,      "+"  },
        LexerPair{ TokenType::op_minus,     "-"  },
        LexerPair{ TokenType::op_multiply,  "*"  },
        LexerPair{ TokenType::op_divide,    "/"  },
        LexerPair{ TokenType::op_remainder, "%"  }
    };


    static const std::array<LexerPair, 11> symbols = {
        LexerPair{ TokenType::sym_period,       "." },
        LexerPair{ TokenType::sym_comma,        "," },
        LexerPair{ TokenType::sym_colon,        ":" },
        LexerPair{ TokenType::sym_semicolon,    ";" },
        LexerPair{ TokenType::sym_questionmark, "?" },
        LexerPair{ TokenType::sym_lparenthesis, "(" },
        LexerPair{ TokenType::sym_rparenthesis, ")" },
        LexerPair{ TokenType::sym_lbracket,     "[" },
        LexerPair{ TokenType::sym_rbracket,     "]" },
        LexerPair{ TokenType::sym_lbrace,       "{" },
        LexerPair{ TokenType::sym_rbrace,       "}" }
    };


    [[maybe_unused]]
    static const std::string& get_token_type_name(TokenType type) {
        static std::string unknown = "<?>";

        if (type >= TokenType::tk_comment && type <= TokenType::tk_hex) {
            return primitives[static_cast<size_t>(type)].second;
        } else if (type >= TokenType::kw_import && type <= TokenType::kw_const) {
            return keywords[static_cast<size_t>(type) - static_cast<size_t>(TokenType::tk_hex)].second;
        } else if (type >= TokenType::op_bit_not && type <= TokenType::op_remainder) {
            return operators[static_cast<size_t>(type) - static_cast<size_t>(TokenType::op_bit_not)].second;
        } else if (type >= TokenType::sym_period && type <= TokenType::sym_rbrace) {
            return symbols[static_cast<size_t>(type) - static_cast<size_t>(TokenType::sym_period)].second;
        }

        return unknown;
    }

}
