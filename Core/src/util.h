#pragma once

#include <fstream>
#include <streambuf>
#include <string>

namespace util {

std::string getFileContent(const std::string& path)
{
    std::ifstream file(path);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    return content;
}
}