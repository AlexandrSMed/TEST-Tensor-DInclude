#include "Utils.hpp"
#include <filesystem>
#include <unordered_set>
#include <regex>
#include <stdexcept>

std::vector<tdw::utils::argument_type> tdw::utils::readArguments(int argc, char* argv[], argument_set_type&& optionsWhiteList) {

    std::vector<argument_type> args;
    if(argc == 1) {
        return args;
    }

    for(auto i = 1; i < argc; ++i) {
        auto found = false;
        for(const auto& option : optionsWhiteList) {
            // https://regex101.com/r/oMVOey/1: ^(-I|--include-directory)=?[^\S\r\n]*([\S]+)?
            const auto optionPrefix = "^(-" + option.shortVersion + "|" + "--" + option.longVersion + ")=?";
            const auto optionArgument = R"(\s*([\S]+)?)";
            
            if(option.acceptsArgument) {
                std::cmatch match;
                if(std::regex_search(argv[i], match, std::regex{ optionPrefix + optionArgument }) && match[2].matched) {
                    args.emplace_back(std::make_pair(option, match[2].str()));
                    found = true;
                    break;
                } else if(std::regex_match(argv[i], std::regex{ optionPrefix }) && (i < argc - 1)) {
                    const auto argument = argv[++i];
                    args.emplace_back(std::make_pair(option, argument));
                    found = true;
                }
            } else if(std::regex_match(argv[i], std::regex{ optionPrefix })) {
                args.emplace_back(std::make_pair(option, ""));
                found = true;
            }
           
        }

        if(found) {
            continue;
        }

        args.emplace_back(argv[i]);
    }
    return args;
}

void tdw::utils::assertCompliantArguments(const std::vector<argument_type>& arguments) {
    if(!arguments.size()) {
        throw std::invalid_argument{ "Please specify a path to the source files" };
    }

    if(!std::holds_alternative<std::string>(arguments[0])) {
        throw std::invalid_argument{ "The first argument must be a path" };
    }

    for(auto it = arguments.cbegin() + 1; it != arguments.cend(); ++it) {
        if(!std::holds_alternative<option_type>(*it)) {
            throw std::invalid_argument{ "Could not read the include path argument: \"" + std::get<std::string>(*it) + "\"" };
        }
        
        const auto& path = std::get<option_type>(*it).second;
        if(!std::filesystem::is_directory(path)) {
            throw std::invalid_argument{ "Include option should refer to an existing directory: \"" + path + "\"" };
        }
    }
}
