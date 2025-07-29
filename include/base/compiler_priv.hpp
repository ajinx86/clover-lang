#pragma once

#include <base/diagnostic.hpp>
#include <base/source.hpp>

#include <lexer/lexer_priv.hpp>
#include <parser/ast.hpp>


namespace clover {

    struct CompileUnit {
        Source src;

        // lexer
        std::vector<Token> tokens;

        // parser
        std::shared_ptr<RootNode> ast;

        explicit CompileUnit(const std::string& _file) : src(_file) {}
    };

    struct CompilerState {
        std::vector<CompileUnit> units;

        bool error = false;

        CompilerState(const std::vector<std::string>& files) {
            if (files.size() == 0) {
                throw std::runtime_error("no input files provided");
            }

            for (auto& file : files) {
                units.push_back(CompileUnit(file));
            }
        }
    };


}
