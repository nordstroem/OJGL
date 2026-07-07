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
    static ojstd::unordered_map<ojstd::string, ShaderContent> _shaders;
};
}
