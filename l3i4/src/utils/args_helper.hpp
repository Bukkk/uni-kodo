#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace utils {

class args_helper {
    std::vector<std::string> found_positional_ {};
    std::vector<std::string> requested_option_ {};
    std::vector<std::string> requested_boolean_ {};

    mutable std::unordered_map<std::string, std::string> option_ {};
    mutable std::unordered_map<std::string, bool> boolean_ {};

    std::string help_page_ { };

public:
    args_helper(std::vector<std::string> options, std::vector<std::string> booleans, std::string help_page)
        : requested_option_(options)
        , requested_boolean_(booleans)
        , help_page_(help_page)
    {
    }

    void parse(int argc, char** argv)
    {
        int i { 1 };
        while (i < argc) {
            std::string arg { argv[i] };

            if (arg.size() > 0 && arg[0] == '-') {

                if (std::find(requested_boolean_.begin(), requested_boolean_.end(), arg) != requested_boolean_.end()) {
                    boolean_[arg] = true;

                    ++i;
                    continue;
                }

                if (i + 1 < argc) {
                    std::string next_arg { argv[i + 1] };

                    if (std::find(requested_option_.begin(), requested_option_.end(), arg) != requested_option_.end()) {
                        option_[arg] = next_arg;

                        i += 2;
                        continue;
                    }
                }

                // nie odnaleziono argumentu
                throw std::runtime_error("nie odnaleziono opcji \'" + arg + "\'");
            } else if (arg.size() > 0) {
                found_positional_.push_back(arg);
                ++i;
                continue;
            }

            throw std::runtime_error("blad parsowania \'" + arg + "\'");
        }
    }

    auto positional_args() const -> const std::vector<std::string>&
    {
        return found_positional_;
    }

    auto option(std::string val) const -> std::string
    {
        return option_.contains(val) ? option_[val] : "";
    }

    auto boolean(std::string val) const -> bool
    {
        return boolean_.contains(val) ? boolean_[val] : false;
    }

    auto help() const -> const std::string&
    {
        return help_page_;
    }
};
}