#pragma once

#include <sstream>
#include <filesystem>
#include <variant>
#include <unordered_set>

#include <string>
#include <vector>

namespace tdw::utils {

    inline void directoryArgumentAssert(const std::filesystem::path& _path, const std::string& message = std::string{}) {

        using namespace std::filesystem;

        if (is_directory(_path)) {
            return;
        }
        
        if (message.empty()) {
            std::ostringstream osstr;
            osstr << "Could not find a directory with name: " << _path;
            throw std::invalid_argument(osstr.str());
        } else {
            throw std::invalid_argument(message);
        }
    }

    // The template is only to ignore the discarded parts of constexpr check. Please do not alter its arguments
    template<typename path_string = typename std::filesystem::path::string_type>
    inline bool operator==(const std::string& _str, const path_string& _pStr) {
        // TODO: implement a robust platform-specific comparison. This works only for trivial cases
        if constexpr (std::is_same_v<std::string::value_type, typename path_string::value_type>) {
            return _str.compare(_pStr.c_str()) == 0;
        } else {
            if (_str.size() != _pStr.size()) {
                return false;
            }

            for (auto i = static_cast<std::string::size_type>(0); i < _str.size(); ++i) {
                if (_str[i] != _pStr[i]) {
                    return false;
                }
            }

            return true;
        }
    }

    struct CommandLineOption {
        std::string shortVersion;
        std::string longVersion;
        bool acceptsArgument;

        struct HashFunction {
            size_t operator()(const CommandLineOption& _option) const {
                const auto shortHash = std::hash<std::string>()(_option.shortVersion);
                const auto longHash = std::hash<std::string>()(_option.longVersion);
                const auto acceptsHash = static_cast<size_t>(_option.acceptsArgument) << 1;
                return shortHash ^ longHash ^ acceptsHash;
            }
        };

        struct EqualTo {
            bool operator()(const CommandLineOption& _left, const CommandLineOption& _right) const {
                return std::operator==(_left.shortVersion, _right.shortVersion) &&
                    std::operator==(_left.longVersion, _right.longVersion) &&
                    (_left.acceptsArgument == _right.acceptsArgument);
            }
        };
    };
    using option_type = typename std::pair<CommandLineOption, std::string>;
    using argument_type = typename std::variant<option_type, std::string>;
    using argument_set_type = typename std::unordered_set<CommandLineOption, CommandLineOption::HashFunction, CommandLineOption::EqualTo>;
    std::vector<argument_type> readArguments(int argc, char* argv[], argument_set_type&& optionsWhiteList = argument_set_type{
        { "I", "include-directory", true }
    });

    void assertCompliantArguments(const std::vector<argument_type>& arguments);

}
