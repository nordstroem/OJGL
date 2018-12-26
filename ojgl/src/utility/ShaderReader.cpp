#include "ShaderReader.h"
#include "Log.h"
#include "utility/Macros.h"
#include "utility/OJstd.h"
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
long long modifyTime(const ojstd::string& path)
{
    struct stat fileStat;
    return stat(path.c_str(), &fileStat) == 0 ? fileStat.st_mtime : 0;
}
#endif
}

void ShaderReader::preLoad(const ojstd::string& path, const ojstd::string& content)
{
    ShaderReader::_shaders[path].content = content;
}

void ShaderReader::setBasePath(const ojstd::string& basePath)
{
    ShaderReader::_basePath = basePath;
}

bool ShaderReader::modified(const ojstd::string& path)
{
#ifdef _DEBUG
    return modifyTime(ShaderReader::_basePath + path) != ShaderReader::_shaders[path].modifyTime;
#else
    OJ_UNUSED(path);
    return false;
#endif
}

const ojstd::string& ShaderReader::get(const ojstd::string& path)
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
        ShaderReader::_shaders[path].content = ojstd::string(shader.c_str());
        ShaderReader::_shaders[path].modifyTime = modifyTime(fullPath);
    }
#endif
    return ShaderReader::_shaders[path].content;
}

ojstd::unordered_map<ojstd::string, ShaderContent> ShaderReader::_shaders;
ojstd::string ShaderReader::_basePath;