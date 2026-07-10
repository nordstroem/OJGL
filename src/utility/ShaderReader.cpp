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

bool fileExists(const ojstd::string& path)
{
    struct stat fileStat;
    return stat(path.c_str(), &fileStat) == 0;
}

// .fs/.vs files store their GLSL inside an R""( ... )"" raw-string wrapper (so they can also be
// #include'd as string literals into the embedded resources). Strip the wrapper when reading
// from disk.
// Returns an empty string if the wrapper isn't found intact, e.g. when hot reload catches the
// file mid-write (a partial save can be missing the opening/closing marker at read time).
ojstd::string unwrapRawString(const std::string& fileContents)
{
    std::string pre = "R\"\"(";
    std::string post = ")\"\"";
    size_t start = fileContents.find(pre);
    size_t end = fileContents.rfind(post);
    if (start == std::string::npos || end == std::string::npos || end < start + pre.length())
        return ojstd::string();
    return fileContents.substr(start + pre.length(), end - start - pre.length()).c_str();
}
#endif

ojstd::string replaceIncludes(const ojstd::string& rawShader)
{
    ojstd::string includeKeyword = R""(#include ")"";
    ojstd::string modifiedShader = rawShader;

    int startPos = 0;
    int includeStart = rawShader.find(includeKeyword, startPos);
    while (includeStart != -1) {
        int nameStart = includeStart + includeKeyword.length();
        int nameEnd = nameStart + 1;
        while (rawShader[nameEnd] != '"')
            nameEnd++;

        int includeStringLength = includeKeyword.length() + nameEnd - nameStart + 2;
        ojstd::string name = rawShader.substring(nameStart, nameEnd);
        ojstd::string includeString = rawShader.substring(includeStart, includeStart + includeStringLength);

        const auto& shaderToInclude = ShaderReader::get(name);
        modifiedShader = modifiedShader.replaceFirst(includeString, shaderToInclude);
        startPos += includeStringLength;
        includeStart = rawShader.find(includeKeyword, startPos);
    }

    return modifiedShader;
}

}

void ShaderReader::preLoad(const ojstd::string& path, const ojstd::string& content)
{
    _ASSERT_EXPR(!ShaderReader::_shaders.contains(path), ojstd::wstringWrapper(path + " is already preloaded.").ptr);
    ShaderContent& entry = ShaderReader::_shaders[path];
    entry.rawContent = content;
    entry.resolved = false;
}

#ifdef _DEBUG
void ShaderReader::registerDiskPath(const ojstd::string& path, const ojstd::string& diskPath)
{
    ShaderReader::_shaders[path].diskPath = diskPath;
}
#endif

bool ShaderReader::modified(const ojstd::string& path)
{
#ifdef _DEBUG
    ShaderContent& entry = ShaderReader::_shaders[path];
    return entry.diskPath.length() != 0 && modifyTime(entry.diskPath) != entry.modifyTime;
#else
    OJ_UNUSED(path);
    return false;
#endif
}

const ojstd::string& ShaderReader::get(const ojstd::string& path)
{
    _ASSERT_EXPR(ShaderReader::_shaders.contains(path), ojstd::wstringWrapper(path + " is not preloaded.").ptr);

#ifdef _DEBUG
    // Reload the raw source from disk when the file has changed (hot reload).
    {
        ShaderContent& entry = ShaderReader::_shaders[path];
        if (entry.diskPath.length() != 0 && fileExists(entry.diskPath)) {
            const long long diskTime = modifyTime(entry.diskPath);
            if (diskTime != entry.modifyTime) {
                std::ifstream shaderFile;
                // Enable exceptions to try and get more info about why the shader reader sometimes fails when reloading
                std::ios_base::iostate exceptionMask = shaderFile.exceptions() | std::ios::failbit;
                shaderFile.exceptions(exceptionMask);

                try {
                    shaderFile.open(entry.diskPath.c_str());
                } catch (std::ios_base::failure& e) {
                    std::cerr << "ShaderReader failed: " << e.what() << '\n';
                }

                if (!shaderFile.fail()) {
                    std::stringstream buffer;
                    buffer << shaderFile.rdbuf();
                    ojstd::string unwrapped = unwrapRawString(buffer.str());
                    if (unwrapped.length() == 0) {
                        // Leave entry.modifyTime untouched so the next modified()/get() check
                        // retries the read once the file's write has settled.
                        LOG_INFO("[" << path.c_str() << "]"
                                     << " reload skipped: raw-string wrapper not found (file mid-write?)");
                    } else {
                        LOG_INFO("[" << path.c_str() << "]"
                                     << " modified");
                        entry.rawContent = unwrapped;
                        entry.modifyTime = diskTime;
                        entry.resolved = false;
                    }
                } else {
                    LOG_INFO("Shader reading failed");
                }
            }
        }
    }
#endif

    // Resolve #include directives lazily and cache the result. Registration order is irrelevant:
    // replaceIncludes calls get() for each included shader, resolving it on demand.
    if (!ShaderReader::_shaders[path].resolved) {
        ojstd::string raw = ShaderReader::_shaders[path].rawContent;
        ojstd::string resolved = replaceIncludes(raw);
        ShaderReader::_shaders[path].resolvedContent = resolved;
        ShaderReader::_shaders[path].resolved = true;
    }
    return ShaderReader::_shaders[path].resolvedContent;
}

ojstd::unordered_map<ojstd::string, ShaderContent> ShaderReader::_shaders;
