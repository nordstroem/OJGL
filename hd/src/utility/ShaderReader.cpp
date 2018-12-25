#include "ShaderReader.h"
#include "Log.h"

#include "../../fakelibs/fakelib.h"
#ifdef _DEBUG
#include <fstream>
#include <sstream>
#include <streambuf>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#endif

using namespace ojgl;

namespace {
#ifdef _DEBUG
long long modifyTime(const fl::string& path)
{
    struct stat fileStat;
    return stat(path.c_str(), &fileStat) == 0 ? fileStat.st_mtime : 0;
}
#endif
}

void ShaderReader::preLoad(const fl::string& path, const fl::string& content)
{
    ShaderReader::_shaders[path].content = content;
}

void ShaderReader::setBasePath(const fl::string& basePath)
{
    ShaderReader::_basePath = basePath;
}

bool ShaderReader::modified(const fl::string& path)
{
#ifdef _DEBUG
    return modifyTime(ShaderReader::_basePath + path) != ShaderReader::_shaders[path].modifyTime;
#else
    return false;
#endif
}

const fl::string& ShaderReader::get(const fl::string& path)
{
#ifdef _DEBUG
    if (modified(path)) {
        auto fullPath = _basePath + path;
        LOG_INFO("[" << path.c_str() << "]"
                     << " modified");
        std::ifstream shaderFile(fullPath.c_str());
        _ASSERTE(!shaderFile.fail());

        std::stringstream buffer;
        buffer << shaderFile.rdbuf();
        std::string fileContents = buffer.str();
        std::string pre = "R\"\"(";
        std::string post = ")\"\"";
        size_t start = fileContents.find(pre);
        size_t end = fileContents.rfind(post);
        std::string shader = fileContents.substr(start + pre.length(), end - start - pre.length());
        ShaderReader::_shaders[path].content = fl::string(shader.c_str());
        ShaderReader::_shaders[path].modifyTime = modifyTime(fullPath);
    }
#else
    auto itr = ShaderReader::_shaders.find(path);
    _ASSERTE(itr != ShaderReader::_shaders.end());
#endif
    return ShaderReader::_shaders[path].content;
}

fl::unordered_map<fl::string, ShaderContent> ShaderReader::_shaders;
fl::string ShaderReader::_basePath;
