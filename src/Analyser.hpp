#pragma once

#include <map>
#include <iostream>
#include <unordered_set>
#include <filesystem>

namespace tdw {

    class Analyser {
    public:
        using path_type = typename std::filesystem::path;

    private:
        /**
         * @brief `first` of the pair denotes the presentational part (which is used to print the entity in the user-friendly manner)
         * I.e. it should be recognizable and kept in the form originally found in the source files, if the record originates from within
         * an `#include` directive or a source file path relative to the given source files folder argument
         * `second` *must* contain the complement part, required to form a (weakly) canonical path to the source file. It's used to uniquely
         * differentiate between keys.
        */
        using include_map_key_type = typename std::pair<path_type, path_type>;
        struct IncludeMapKeyLess {
            bool operator()(const include_map_key_type& _left, const include_map_key_type& _right) const {
                // Checks whether the it's the same values;
                const auto leftPath = weakly_canonical(_left.second / _left.first);
                const auto rightPath = weakly_canonical(_right.second / _right.first);
                if(leftPath == rightPath) {
                    return false;
                }

                // Lexical comparison (alphabetical) for "short" names;
                return _left.first < _right.first;
            }
        };
        struct IncludeMapKeyHash {
            size_t operator()(const include_map_key_type& _include) const {
                return std::hash<path_type>()(weakly_canonical(_include.second / _include.first));
            }
        };
        struct IncludeMapKeyEqualTo {
            bool operator()(const include_map_key_type& _left, const include_map_key_type& _right) const {
                const auto leftPath = weakly_canonical(_left.second / _left.first);
                const auto rightPath = weakly_canonical(_right.second / _right.first);
                return leftPath == rightPath;
            }
        };
        using include_counter_map_type = std::map<include_map_key_type, unsigned, IncludeMapKeyLess>;
        using include_chain_set_type = std::unordered_set<include_map_key_type, IncludeMapKeyHash, IncludeMapKeyEqualTo>;

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
        /**
         * @brief Prints dependency tree for the given `Include` argument with respect to current and include paths.
         * @param _sourceFile - the include statement
         * @param _currentPath - the relative "current" directore search needs to be done in relation to
         * @param _includePaths - include directories to look for includes in
         * @param _includeCounter - a collection keeping track of includes number for the given argument
         * @param _includeChain - a collection keeping track of the current include chain
         * @param _depth - current depth of include chain
         * @return `path_type` of the directory the given include was found in. The path is empty, if its parent was not found
        */
        static path_type printDependencyTree(const Include& _sourceFile,
                                             const path_type _currentPath,
                                             const std::vector<path_type>& _includePaths,
                                             include_counter_map_type& _includeCounter,
                                             include_chain_set_type& _includeChain,
                                             unsigned _depth = 0);

        static inline void printIncludeBranchRecord(path_type _path, unsigned _depth, bool _found, bool _cycleInclude, path_type relative_to_path = path_type{}) {
            if(_depth) {
                std::cout << std::string(_depth, '_'); // Underscorde instead of dot for the better distinctions with special paths ("." and "..")
            }
            
            if(relative_to_path.empty()) {
                std::cout << _path; // TODO: add rationale behind not using "make_preferred()", i.e. to keep the output consistent with include directive
            } else {
                std::cout << std::filesystem::relative(_path, relative_to_path);
            }
            
            if(!_found) {
                std::cout << "(!)";
            }

            if(_cycleInclude) {
                std::cout << "(~)";
            }
            std::cout << std::endl;
        }
        

        const path_type path;
        // Container of source files, with absolute paths
        source_files_type sourceFiles;
    public:
        explicit Analyser(const path_type& _path);
        void printDependencyTree(const std::vector<path_type>& _includePaths) const;

    };
}
