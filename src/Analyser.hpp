#pragma once

#include <map>
#include <iostream>
#include <unordered_set>
#include <filesystem>

namespace tdw {

    class Analyser {

        using path_type = typename std::filesystem::path;
        // Different includes (i.e. `"../include/foo.h"` and <foo.h>), may in fact represent the same entity, but in order
        // to print the data properly we have to store the path in relation to which the include was originally found
        // (relative vs. arguments)
        using include_map_key_type = typename std::pair<path_type, path_type>;
        struct IncludeMapKeyLess {
            bool operator()(const include_map_key_type& _left, const include_map_key_type& _right) const {
                // Checks whether the it's the same values;
                const auto leftPath = canonical(_left.second / _left.first);
                const auto rightPath = canonical(_right.second / _right.first);
                if(leftPath == rightPath) {
                    return false;
                }

                // Lexical comparison (alphabetical) for "short" names;
                return _left.first < _right.first;
            }
        };
        using include_counter_map_type = std::map<include_map_key_type, unsigned char, IncludeMapKeyLess>;

        struct Include {
            enum class Type {
                q_char, h_char, pp_tokens
            };

            const path_type path;
            const Type type;

            Include(const path_type& _path, Type _type) : path{ _path }, type{ _type } {}

            struct HashFunction {
                size_t operator()(const Include& _include) const {
                    const auto pathHash = std::hash<path_type>()(_include.path);
                    const auto typeHash = std::hash<Type>()(_include.type) << 1;
                    return pathHash ^ typeHash;
                }
            };

            struct EqualTo {
                bool operator()(const Include& _left, const Include& _right) const {
                    return (_left.path == _right.path) && (_left.type == _right.type);
                }
            };
        };
        using source_files_type = std::unordered_set<Include, Include::HashFunction, Include::EqualTo>;

        static std::vector<Include> getIncludes(const path_type& _path);
        /**
         * @return `path_type` the source was found in or empty `path_type` it search fails
        */
        static path_type findIncludeParentPath(const Include& _sourceFile, const path_type _currentPath, const std::vector<path_type>& _includePaths);
        static void printDependencyTree(const Include& _sourceFile, const path_type _currentPath, const std::vector<path_type>& _includePaths, include_counter_map_type& _includeCounter, unsigned _depth = 0);
        static inline void printIncludeBranchRecord(path_type _path, unsigned _depth, bool _found, path_type relative_to_path = path_type{}) {
            if(_depth) {
                std::cout << std::string(_depth, '.');
            }
            
            if(relative_to_path.empty()) {
                std::cout << _path; // TODO: add rationale behind not using "make_preferred()", i.e. to keep the output consistent with include directive
            } else {
                std::cout << std::filesystem::relative(_path, relative_to_path);
            }
            
            if(!_found) {
                std::cout << "(!)";
            }
            std::cout << std::endl;
        }
        

        const path_type path;
        source_files_type sourceFiles;
    public:
        explicit Analyser(const path_type& _path);
        void printDependencyTree(const std::vector<path_type>& _includePaths) const;

    };
}
