#include "Analyser.hpp"
#include "Utils.hpp"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <regex>

std::vector<tdw::Analyser::Include> tdw::Analyser::getIncludes(const path_class& _path) {
    std::ifstream ifs;
    // Makes the `ifstream` throw an exception in case it fails to open the file
    ifs.exceptions(ifs.exceptions() | std::ios::failbit);
    ifs.open(_path);
    using if_stream_buf_it = typename std::istreambuf_iterator<std::ifstream::char_type>;
    const std::string fileData{ (if_stream_buf_it(ifs)), if_stream_buf_it() };
    ifs.close();

    // TODO: fix for scenario of multline comment/raw str that extends to end of string https://regex101.com/r/AnnCDl/1
    const std::regex noiseRegex{ R"reg(((?:R"(.*)\()(?:[\s\S\n]*?)(?:\)\2")|(?:\/\*)(?:[\s\S\n]*?)(?:\*\/)))reg" };
    const auto filteredFileData = std::regex_replace(fileData, noiseRegex, "");

    const std::regex includeRegex{ R"reg(^[^\S\r\n]*#[^\S\r\n]*include[^\S\r\n]*("([\w.\/\\]+(\.hpp|\.cpp))"|\<([\w.\/\\]+(\.hpp|\.cpp))\>))reg" };
    constexpr auto quoteMatchIndex = static_cast<std::smatch::size_type>(2);
    constexpr auto bracketMatchIndex = static_cast<std::smatch::size_type>(4);

    std::vector<Include> includes;
    for (std::sregex_iterator it(filteredFileData.cbegin(), filteredFileData.end(), includeRegex); it != std::sregex_iterator{}; ++it) {
        const auto& match = *it;

        if (match.size() <= std::max(quoteMatchIndex, bracketMatchIndex)) {
            continue;
        }

        if (match[quoteMatchIndex].matched) {
            includes.emplace_back(match.str(quoteMatchIndex), true);
        } else if (match[bracketMatchIndex].matched) {
            includes.emplace_back(match.str(bracketMatchIndex), false);
        }
    }

    return includes;
}

tdw::Analyser::Analyser(const path_class& _path) : path{ std::filesystem::absolute(_path) } {
    using namespace std::filesystem;
    using utils::operator==;

    utils::directoryArgumentAssert(path, "The source path is not a directory");

    std::unordered_map<path_class, std::vector<Include>> tmp;
    for (const auto& entry : recursive_directory_iterator(path)) {
        const auto& extension = entry.path().extension().native();

        const auto comparisonResult = (".hpp" == extension) || (".cpp" == extension);
        if (!entry.is_regular_file() || !comparisonResult) {
            // Ignore folders, irregular and irrelevant files
            continue;
        }

        tmp[absolute(entry.path())] = getIncludes(entry.path());
    }
    sourceFiles = std::move(tmp);
}


void tdw::Analyser::printDependencyTree(std::initializer_list<path_class> _headerPaths) const {
    using namespace std::filesystem;

    for (const auto& source : sourceFiles) {
        std::cout << relative(source.first, path) << std::endl;
    }

}
