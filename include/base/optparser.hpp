#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <optional>
#include <stdexcept>
#include <functional>


namespace clover {

    using std::string_literals::operator""s;

    typedef std::function<void()> OptionCallbackFunc;

    struct Option {
        const std::string option;
        const bool takes_arg;

        const OptionCallbackFunc callback;

        Option(const std::string& _option) :
            option(_option), takes_arg(false), callback(noop_callback) {}

        Option(const std::string& _option, OptionCallbackFunc _callback) :
            option(_option), takes_arg(false), callback(_callback) {}

        Option(const std::string& _option, bool _takes_arg) :
            option(_option), takes_arg(_takes_arg), callback(noop_callback) {}

    private:
        static void noop_callback() {}
    };

    class OptParser {
    public:
        OptParser(const std::vector<Option>& rules) : _rules(rules) {}

        void parse(int argc, const char **argv) {
            bool end_options = false;

            for (int i = 1; i < argc; i++) {
                const char *curr = argv[i];

                if (curr == "--"s) {
                    end_options = true;
                    continue;
                }

                if (!end_options && is_option(argv[i])) {
                    auto rule = find_rule(curr);

                    if (!rule) {
                        throw std::invalid_argument("unknown option: "s + argv[i]);
                    }

                    Option opt = rule.value();

                    if (opt.takes_arg) {
                        if (i + 1 >= argc) {
                            throw std::invalid_argument("option requires an argument: "s + argv[i]);
                        }

                        _kv_options[opt.option] = argv[++i];
                    } else if (opt.callback) {
                        opt.callback();
                    } else {
                        _options.push_back(argv[i]);
                    }
                } else {
                    _args.push_back(curr);
                }
            }
        }

        const std::optional<std::string> get_value(const std::string& kv_option) const {
            if (!is_option(kv_option)) {
                throw std::invalid_argument("argument is not an option: "s + kv_option);
            }

            if (_kv_options.find(kv_option) != _kv_options.end()) {
                return _kv_options.at(kv_option);
            }

            return std::nullopt;
        }

        inline bool get_option(const std::string& option) const {
            if (is_option(option)) {
                return std::find(_options.begin(), _options.end(), option) != _options.end();
            }

            return false;
        }

        inline const std::vector<std::string>& get_args() const { return _args; }

        inline const std::vector<Option>& rules() const { return _rules; }

    private:
        inline bool is_option(const std::string& arg) const {
            return (!arg.empty() && arg[0] == '-');
        }

        inline std::optional<Option> find_rule(const std::string &arg) {
            for (const auto& rule : _rules) {
                if (rule.option == arg) {
                    return rule;
                }
            }

            return std::nullopt;
        }

    private:
        const std::vector<Option> _rules;

        std::unordered_map<std::string, std::string> _kv_options;
        std::vector<std::string> _options;
        std::vector<std::string> _args;
    };

}
