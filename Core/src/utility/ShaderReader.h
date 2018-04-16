#pragma once

#include <unordered_map>

namespace ojgl {

struct ShaderContent {
    std::string content;
    long long modifyTime;
};

class ShaderReader {

public:
    static void preLoad(const std::string& path, const std::string& content);
    static void setBasePath(const std::string& basePath);
    static bool modified(const std::string& path);
    static const std::string& get(const std::string& path);

private:
    static std::unordered_map<std::string, ShaderContent> _shaders;
    static std::string _basePath;
};
}
