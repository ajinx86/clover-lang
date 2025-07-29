#include <parser/parser.hpp>
#include <parser/parser_priv.hpp>

#include <lexer/lexer_consts.hpp>

#include <base/compiler.hpp>
#include <base/compiler_priv.hpp>


using std::operator""s;

using clover::TokenType;
using clover::Token;

using clover::Parser;
using clover::ParserFunc;
using clover::ParserError;
using clover::make_shared_ptr_cast;


clover::Node *parse_any(Parser& p, const std::initializer_list<ParserFunc>& accepts) {
    for (auto& parse_fn : accepts) {
        clover::Node *node = parse_fn(p);

        if (node != nullptr) {
            return node;
        }
    }

    clover::diagnostic::error("unexpected token", p.state.loc());
    throw ParserError();
}


static clover::ImportStmt *parse_import_stmt(Parser& p) {
    if (!p.state.advance(TokenType::kw_import)) {
        return nullptr;
    }

    auto _import = new clover::ImportStmt();

    p.capture(TokenType::tk_identifier, _import->name);
    p.expect(TokenType::sym_semicolon);

    return _import;
}


static clover::FnDecl *parse_fn_decl(Parser& p) {
    if (!p.state.advance(TokenType::kw_fn)) {
        return nullptr;
    }

    auto _fn = new clover::FnDecl();

    p.capture(TokenType::tk_identifier, _fn->name);
    p.parse_params(_fn->params);

    if (p.state.advance(TokenType::sym_colon)) {
        p.parse_type(_fn->rtype);
    }

    p.parse_fn_block(_fn->body);

    return _fn;
}


static clover::RootNode *parse_root(Parser& p) {
    auto root = new clover::RootNode();

    while (!p.state.eof()) {
        auto node = parse_any(p, { parse_import_stmt, parse_fn_decl });

        switch (node->type) {
            case clover::NodeType::import_stmt:
                root->imports.push_back(make_shared_ptr_cast<clover::ImportStmt>(node));
                break;
            case clover::NodeType::fn_decl:
                root->functions.push_back(make_shared_ptr_cast<clover::FnDecl>(node));
                break;
            default:
                delete root;
                root = nullptr;
        }
    }

    return root;
}


bool clover::parse(CompileUnit& unit) {
    Parser p(unit);

    try {
        unit.ast = make_shared_ptr_cast<RootNode>(parse_root(p));
    } catch (ParserError&) {
        return false;
    }

    return true;
}
