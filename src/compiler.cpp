#include <base/compiler.hpp>
#include <base/compiler_priv.hpp>

#include <lexer/lexer.hpp>
#include <parser/parser.hpp>

#include <memory>


// [[maybe_unused]]
// static void dump_units(std::vector<std::shared_ptr<CompileUnit>>& units) {
//     for (auto& unit : units) {
//         std::cout << "DUMPING: " << unit->file << "\n\n";

//         for (auto& token : unit->tokens) {
//             std::cout << "- value: " << std::string(token.value)
//                 << ", off: " << token.offset
//                 << ", len: " << token.length
//                 << ", line: " << token.line
//                 << ", column: " << token.column
//                 << ", type: " << static_cast<int>(token.type)
//                 << "\n";
//         }

//         std::cout << std::flush;
//     }
// }


bool clover::compile(const std::vector<std::string>& input_files, const std::string& output, BuildFlags& flags) {
    CompilerState st(input_files);

    for (auto& unit : st.units) {
        if (!lex(unit)) {
            st.error = true;
            break;
        }

        if (!parse(unit)) {
            st.error = true;
            break;
        }
    }

    // dump_units(st.units);
    //! TODO: dump_tree(st.ast);

    return !st.error;
}
