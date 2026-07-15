#pragma once

#include "utility/OJstd.h"

namespace ojgl {

struct ShaderContent {
    ojstd::string rawContent; // shader source as loaded (embedded literal, or re-read from disk in debug)
    ojstd::string resolvedContent; // rawContent with #include directives expanded; valid when resolved
    bool resolved = false;
    long long modifyTime = 0LL;
#ifdef _DEBUG
    ojstd::string diskPath; // absolute path on disk, for hot reload
    ojstd::vector<ojstd::string> includes; // direct #include virtual paths, recorded at last resolve
#endif
};

class ShaderReader {

public:
    // Register a shader's raw source under its virtual path (e.g. "common/quad.vs"). Includes are
    // resolved lazily on first get(), so the registration order does not matter.
    static void preLoad(const ojstd::string& path, const ojstd::string& content);
#ifdef _DEBUG
    // Map a virtual path to its absolute on-disk location so get()/modified() can hot reload it.
    static void registerDiskPath(const ojstd::string& path, const ojstd::string& diskPath);
#endif
    static bool modified(const ojstd::string& path);
    static const ojstd::string& get(const ojstd::string& path);

private:
#ifdef _DEBUG
    // Recursively re-read `path` and everything it (transitively) #includes from disk, invalidating
    // the resolved cache of any entry whose own file or a transitive include changed. Returns true if
    // `path`'s resolved content needs rebuilding.
    static bool refreshFromDisk(const ojstd::string& path, ojstd::unordered_set<ojstd::string>& visited);
    // Returns true if `path`'s own file or any of its transitive includes changed on disk.
    static bool modifiedRecursive(const ojstd::string& path, ojstd::unordered_set<ojstd::string>& visited);
#endif
    static ojstd::unordered_map<ojstd::string, ShaderContent> _shaders;
};
}
