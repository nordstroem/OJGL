#pragma once

#include "../../fakelibs/fakelib.h"

namespace ojgl {

struct ShaderContent {
    fl::string content;
    long long modifyTime;
};

class ShaderReader {

public:
    static void preLoad(const fl::string& path, const fl::string& content);
    static void setBasePath(const fl::string& basePath);
    static bool modified(const fl::string& path);
    static const fl::string& get(const fl::string& path);

private:
    static fl::unordered_map<fl::string, ShaderContent> _shaders;
    static fl::string _basePath;
};
}
