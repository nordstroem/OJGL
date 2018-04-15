#include "ShaderReader.h"
#include "Log.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <streambuf>
#include <sys/stat.h>
#include <sys/types.h>

using namespace ojgl;

namespace {
long long modifyTime(const std::string& path)
{
    struct stat fileStat;
    return stat(path.c_str(), &fileStat) == 0 ? fileStat.st_mtime : 0;
}
}

void ShaderReader::preLoad(const std::string& path, const std::string& content)
{
    ShaderReader::_shaders[path].content = content;
}

void ShaderReader::setBasePath(const std::string& basePath)
{
    ShaderReader::_basePath = basePath;
}

bool ShaderReader::modified(const std::string& path)
{
#ifdef _DEBUG
    return modifyTime(ShaderReader::_basePath + path) != ShaderReader::_shaders[path].modifyTime;
#else
    return false;
#endif
}

const std::string& ShaderReader::get(const std::string& path)
{
#ifdef _DEBUG
    if (modified(path)) {
        auto fullPath = _basePath + path;
        LOG_INFO("[" << path << "]"
                     << " modified");
        std::ifstream shaderFile(fullPath);
        _ASSERTE(!shaderFile.fail());

        std::stringstream buffer;
        buffer << shaderFile.rdbuf();
        std::string fileContents = buffer.str();
        std::string pre = "R\"\"(";
        std::string post = ")\"\"";
        size_t start = fileContents.find(pre);
        size_t end = fileContents.rfind(post);
        std::string shader = fileContents.substr(start + pre.length(), end - start - pre.length());
        ShaderReader::_shaders[path].content = shader;
        ShaderReader::_shaders[path].modifyTime = modifyTime(fullPath);
    }
#else
    auto itr = ShaderReader::_shaders.find(path);
    _ASSERTE(itr != ShaderReader::_shaders.end());
#endif
    return ShaderReader::_shaders[path].content;
}

std::unordered_map<std::string, ShaderContent> ShaderReader::_shaders;
std::string ShaderReader::_basePath;
