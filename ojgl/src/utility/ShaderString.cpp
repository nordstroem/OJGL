#include "ShaderString.h"
#include "utility/Log.h"
#include <fstream>
#include <sstream>
#include <streambuf>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>

using namespace ojgl;

static ojstd::string readFile(const ojstd::string& path)
{
    std::ifstream shaderFile;
    // Enable exceptions to try and get more info about why the shader reader sometimes fails when reloading
    std::ios_base::iostate exceptionMask = shaderFile.exceptions() | std::ios::failbit;
    shaderFile.exceptions(exceptionMask);

    try {
        shaderFile.open(path.c_str());
    } catch (std::ios_base::failure& e) {
        std::cerr << "ShaderReader failed: " << e.what() << '\n';
    }
    std::stringstream buffer;
    buffer << shaderFile.rdbuf();
    std::string fileContents = buffer.str();
    std::string pre = "R\"\"(";
    std::string post = ")\"\"";
    size_t start = fileContents.find(pre);
    size_t end = fileContents.rfind(post);
    return fileContents.substr(start + pre.length(), end - start - pre.length()).c_str();
}

ShaderString::ShaderString(const ojstd::string& path)
    : _path(path)
{
}

ojstd::string ShaderString::get() const
{
    /* const auto baseShader = parse();

    for (const auto& include : _includes) {
        const auto& shader = include.shaderString->get();
    }
    */
    return "";
}

struct ShaderContent {
    ojstd::string base;
    ojstd::vector<Include> includes;
};

ojstd::string ShaderString::parse(const ojstd::string& path) const
{
    ojstd::string rawFile = readFile(path);

    ojstd::vector<int> cutOffs;
    ojstd::vector<Include> includes;

    int searchPos = 0;
    int totalLength = rawFile.length();
    ojstd::string includeKeyword = R""(#include ")"";

    int offset = 0;
    cutOffs.push_back(0);
    while (searchPos < totalLength) {
        int includePos = rawFile.find(includeKeyword, searchPos);
        if (includePos == -1)
            break;

        int nameStart = includePos + includeKeyword.length();
        int nameEnd = nameStart + 1;
        while (rawFile[nameEnd] != '"')
            nameEnd++;

        cutOffs.push_back(nameStart + 1);
        cutOffs.push_back(nameEnd);

        int includeStringLength = includeKeyword.length() + nameEnd - nameStart + 2;
        ojstd::string includeName = rawFile.substring(nameStart, nameEnd);
        ojstd::string includeString = rawFile.substring(includePos, includePos + includeStringLength);

        Include include;
        include.shaderString = ojstd::make_shared<ShaderString>(includeName);
        include.position = includePos - offset;
        includes.push_back(std::move(include));

        offset += includeStringLength;
    }
    cutOffs.push_back(totalLength);

    /* _ASSERTE(cutOffs.size() % 2 == 0);

    char* strippedBaseFile = (char*)malloc(sizeof(char) * (totalLength - offset + 1));
    _ASSERTE(strippedBaseFile != nullptr);
    strippedBaseFile[totalLength - offset] = '\0';

    int currentPosition = 0;
    int charsWritten = 0;
    for (int i = 0; i < cutOffs.size(); i += 2) {
        int startPosition = cutOffs[i];
        int endPosition = cutOffs[i + 1];
        memcpy(&strippedBaseFile[charsWritten], &rawFile.c_str()[currentPosition], startPosition - currentPosition);
        currentPosition = endPosition;
    }

    LOG_INFO(strippedBaseFile);
    return ojstd::string(std::move(strippedBaseFile));
    */
}
