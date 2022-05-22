#pragma once

#include <unordered_map>
#include <filesystem>

namespace tdw {

    class Analyser {
        using path_class = typename std::filesystem::path;

        struct Include {
            const path_class path;
            const bool isRelative;

            Include(const std::string& _pathStr, bool _relative) : path{ _pathStr }, isRelative{ _relative } {}
        };
        static std::vector<Include> getIncludes(const path_class& _path);

        const path_class path;
        std::unordered_map<path_class, std::vector<Include>> sourceFiles;

    public:
        explicit Analyser(const path_class& _path);
        void printDependencyTree(std::initializer_list<path_class> _headerPaths) const;

         
    };
}
