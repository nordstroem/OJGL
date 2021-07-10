#pragma once
#include "utility/OJstd.h"

namespace ojgl {

class ShaderString;

struct Include {
    ojstd::shared_ptr<ShaderString> shaderString;
    int position;
};

class ShaderString {
public:
    explicit ShaderString(const ojstd::string& path);
    ojstd::string get() const;

    ojstd::string parse(const ojstd::string& path) const;

private:
    ojstd::string _path;
    mutable ojstd::vector<Include> _includes; // Sorted by position
    mutable ojstd::string _expandedFile;
    mutable long long _modifyTime = 0LL;
    const ojstd::string _basePath = "kek";
};
}
