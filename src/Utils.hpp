#pragma once

#include <string>
#include <sstream>
#include <filesystem>
#include <type_traits>

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

}
