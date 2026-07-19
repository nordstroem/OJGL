#pragma once

#include "utility/OJstd.h"

namespace ojgl {

struct ShaderContent {
    ojstd::string rawContent; 
    ojstd::string resolvedContent;
    bool resolved = false;
    long long modifyTime = 0LL;
#ifdef _DEBUG
    ojstd::string diskPath; 
    ojstd::vector<ojstd::string> includes;
#endif
};

class ShaderReader {

public:
    static void preLoad(const ojstd::string& path, const ojstd::string& content);
#ifdef _DEBUG
    static void registerDiskPath(const ojstd::string& path, const ojstd::string& diskPath);
#endif
    static bool modified(const ojstd::string& path);
    static const ojstd::string& get(const ojstd::string& path);

private:
#ifdef _DEBUG
    static bool refreshFromDisk(const ojstd::string& path, ojstd::unordered_set<ojstd::string>& visited);
    static bool modifiedRecursive(const ojstd::string& path, ojstd::unordered_set<ojstd::string>& visited);
#endif
    static ojstd::unordered_map<ojstd::string, ShaderContent> _shaders;
};
}
