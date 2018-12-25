#pragma once

#include "utility/OJstd.h"

namespace ojgl {

struct ShaderContent {
    ojstd::string content;
    long long modifyTime;
};

class ShaderReader {

public:
    static void preLoad(const ojstd::string& path, const ojstd::string& content);
    static void setBasePath(const ojstd::string& basePath);
    static bool modified(const ojstd::string& path);
    static const ojstd::string& get(const ojstd::string& path);

private:
    static ojstd::unordered_map<ojstd::string, ShaderContent> _shaders;
    static ojstd::string _basePath;
};
}
