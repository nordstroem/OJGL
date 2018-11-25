#pragma once

#include ""

namespace ojgl {

struct ShaderContent {
    char* content;
    long long modifyTime;
};

class ShaderReader {

public:
    static void preLoad(const char* path, const char* content);
    static void setBasePath(const std::string& basePath);
    static bool modified(const char* path);
    static const char* get(const char* path);

private:
    static fl::unordered_map<const char*, ShaderContent> _shaders;
    static char* _basePath;
};
}
