#pragma once

#include <string>
#include <vector>
#include <memory>
#include <sstream>


namespace clover {

    enum class NodeType {
        root,
        block,
        mod_spec,
        type_spec,
        param_spec,
        import_stmt,
        fn_decl,
    };


    struct Node {
        NodeType type;

        Node(NodeType _type) : type(_type) {}
        virtual ~Node() {}
        virtual std::string to_string() const { return ""; }
    };


    // == meta nodes


    struct ModSpec : Node {
        struct {
            bool f_static;
            bool f_const;
            bool f_pub;
        };

        ModSpec() : Node(NodeType::mod_spec) {}
        ~ModSpec() override {}

        virtual std::string to_string() const override {
            std::stringstream result;

            result << "mod_spec [";

            if (f_static) { result << "S"; }
            if (f_const)  { result << "C"; }
            if (f_pub)    { result << "P"; }

            result << "]";

            return result.str();
        }
    };


    struct TypeSpec : Node {
        std::string name = "void";
        std::vector<TypeSpec> tparams = {};
        bool nullable = false;

        TypeSpec() : Node(NodeType::type_spec) {}
        ~TypeSpec() override {}
    };


    struct ParamSpec : Node {
        std::string name;

        TypeSpec ptype;
        ModSpec mod;

        ParamSpec() : Node(NodeType::param_spec) {}
        ~ParamSpec() override {}
    };


    struct Block : Node {
        std::vector<std::shared_ptr<Node>> instructions;

        Block() : Node(NodeType::block) {}
        ~Block() override {}
    };


    // == structure nodes


    struct RootNode : Node {
        std::vector<std::shared_ptr<struct ImportStmt>> imports;
        std::vector<std::shared_ptr<struct FnDecl>> functions;
        // std::vector<struct VarDecl> vars;

        RootNode() : Node(NodeType::root) {}
        ~RootNode() override {}
    };


    struct ImportStmt : Node {
        std::string name;

        ImportStmt() : Node(NodeType::import_stmt) {}
        ~ImportStmt() override {}
    };


    struct FnDecl : Node {
        std::string name;
        std::vector<ParamSpec> params;

        TypeSpec rtype;
        Block body;

        FnDecl() : Node(NodeType::fn_decl) {}
        ~FnDecl() override {}
    };


    struct VarDecl : Node {
        std::string name;

        TypeSpec type;

        VarDecl() : Node(NodeType::import_stmt) {}
        ~VarDecl() override {}
    };


    struct ConstDecl : Node {
        std::string name;

        TypeSpec type;

        ConstDecl() : Node(NodeType::import_stmt) {}
        ~ConstDecl() override {}
    };

}
