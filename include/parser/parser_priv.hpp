#pragma once

#include <base/compiler_priv.hpp>
#include <lexer/lexer_consts.hpp>

#include <functional>
#include <memory>


namespace clover {

    struct ParserError {};

    struct ParserState {
        CompileUnit& unit;

        uint32_t curr_token = 0;
        uint32_t prev_token = 0;

        ParserState(CompileUnit& _unit) : unit(_unit) {}

        inline bool eof() const { return curr_token >= unit.tokens.size(); }

        inline Token& previous() const { return unit.tokens[prev_token]; }

        inline Token& current() const { return unit.tokens[curr_token]; }

        inline bool matches(TokenType type) const { return !eof() && current().type == type; }

        inline bool advance(TokenType type) {
            if (current().type == type) {
                prev_token = curr_token;
                curr_token++;
                return true;
            }

            return false;
        }

        inline bool capture(TokenType type, std::string& out) {
            if (matches(type)) {
                out = std::string(current().value);
                curr_token++;
                return true;
            }

            return false;
        }

        inline bool capture_token(TokenType type, Token& out) {
            if (matches(type)) {
                out = current();
                curr_token++;
                return true;
            }

            return false;
        }

        inline diagnostic::Location loc(const std::string& scope = "") const {
            return diagnostic::Location {
                .offset = current().offset,
                .line_offset = current().line_offset,
                .length = current().length,
                .line = current().line,
                .column = current().column,
                .scope = scope,
                .src = unit.src
            };
        }

        inline diagnostic::Location prev_loc(const std::string& scope = "") const {
            return diagnostic::Location {
                .offset = previous().offset,
                .line_offset = previous().line_offset,
                .length = previous().length,
                .line = previous().line,
                .column = previous().column,
                .scope = scope,
                .src = unit.src
            };
        }

    };


    struct Parser {
        ParserState state;

        Parser(ParserState&& _state) : state(_state) {}

        void expect(TokenType type) {
            if (!state.advance(type)) {
                diagnostic::error("expected "s + get_token_type_name(type), state.loc());
                throw ParserError();
            }
        }

        void capture(TokenType type, std::string& out) {
            if (!state.capture(type, out)) {
                diagnostic::error((
                    "expected "s + get_token_type_name(type) +
                    ", got "s + get_token_type_name(state.current().type)), state.loc());
                throw ParserError();
            }
        }

        void capture_token(TokenType type, Token& out) {
            if (state.eof()) {
                diagnostic::error("premature end-of-file", state.loc());
                throw ParserError();
            }

            if (!state.capture_token(type, out)) {
                diagnostic::error((
                    "expected "s + get_token_type_name(type) +
                    ", got "s + get_token_type_name(state.current().type)), state.loc());
                throw ParserError();
            }
        }

        void check_repeat(bool already_present) {
            if (!already_present) {
                return;
            }

            diagnostic::warn("duplicated '"s + state.previous().value + "'", state.prev_loc());
        }

        void parse_mods(ModSpec& out) {
            ModSpec ms;

            while (true) {
                if (state.advance(TokenType::kw_static)) {
                    check_repeat(ms.f_static);
                    ms.f_static = true;
                } else if (state.advance(TokenType::kw_const)) {
                    check_repeat(ms.f_const);
                    ms.f_const = true;
                } else if (state.advance(TokenType::kw_pub)) {
                    check_repeat(ms.f_pub);
                    ms.f_pub = true;
                } else {
                    break;
                }
            }

            out = ms;
        }

        void parse_type(TypeSpec& out) {
            TypeSpec ts{};

            capture(TokenType::tk_identifier, ts.name);

            if (state.advance(TokenType::op_lt)) {
                do {
                    TypeSpec tp{};
                    parse_type(tp);
                    ts.tparams.push_back(tp);
                } while (state.advance(TokenType::sym_comma));

                expect(TokenType::op_gt);
            }

            if (state.advance(TokenType::sym_semicolon)) {
                ts.nullable = true;
            }
        }

        void parse_param(ParamSpec& out) {
            capture(TokenType::tk_identifier, out.name);
            expect(TokenType::sym_colon);
            parse_mods(out.mod);
            parse_type(out.ptype);
        }

        void parse_params(std::vector<ParamSpec>& out) {
            expect(TokenType::sym_lparenthesis);

            do {
                ParamSpec ps{};
                parse_param(ps);
                out.push_back(ps);
            } while (state.advance(TokenType::sym_comma));

            expect(TokenType::sym_rparenthesis);
        }

        void parse_fn_block(Block& out) {
            expect(TokenType::sym_lbrace);

            // const_decl
            // var_decl
            // defer_stmt
            // if_stmt
            // for_stmt
            // while_stmt
            // switch_stmt
            // do_while_stmt
            // return_stmt

            expect(TokenType::sym_rbrace);
        }
    };


    typedef std::function<Node* (Parser&)> ParserFunc;


    template <typename T>
    static inline constexpr std::shared_ptr<T> make_shared_ptr_cast(Node *node) {
        return std::shared_ptr<T>(dynamic_cast<T*>(node));
    }

}
