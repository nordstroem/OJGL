#include "Log.h"
#include "ShaderReader.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <streambuf>
#include <sys/stat.h>
#include <sys/types.h>

using namespace ojgl;

namespace {
long long modifyTime(const char* path)
{
    struct stat fileStat;
    return stat(path, &fileStat) == 0 ? fileStat.st_mtime : 0;
}
}

void ShaderReader::preLoad(const char* path, const char* content)
{
    ShaderReader::_shaders[path].content = content;
}

void ShaderReader::setBasePath(const char* basePath)
{
    ShaderReader::_basePath = basePath;
}

bool ShaderReader::modified(const char* path)
{
#ifdef _DEBUG
    return modifyTime(ShaderReader::_basePath + path) != ShaderReader::_shaders[path].modifyTime;
#else
    return false;
#endif
}

const char* ShaderReader::get(const char* path)
{
#ifdef _DEBUG
    /* if (modified(path))
    {
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
    }*/
#else
    auto itr = ShaderReader::_shaders.find(path);
    _ASSERTE(itr != ShaderReader::_shaders.end());
#endif
    return ShaderReader::_shaders[path].content;
}

fl::unordered_map<const char*, ShaderContent> ShaderReader::_shaders;
char* ShaderReader::_basePath;
