#include "Buffer.h"
#include <iostream>
#include <memory>
#include <string>

namespace ojgl {

Buffer::Buffer(const std::string& name, const std::string& vertex, const std::string& fragment, std::initializer_list<std::shared_ptr<Buffer>> buffers)
    : _inputs(buffers)
    , _name(name)
{
    loadShader(vertex, fragment);
}

Buffer::~Buffer()
{
}

std::shared_ptr<Buffer> Buffer::construct(const std::string& name, const std::string& vertex, const std::string& fragment)
{
    return construct(name, fragment, vertex, {});
}

std::shared_ptr<Buffer> Buffer::construct(const std::string& name, const std::string& vertex, const std::string& fragment, std::initializer_list<std::shared_ptr<Buffer>> buffers)
{
    return std::shared_ptr<Buffer>(new Buffer(name, fragment, vertex, buffers));
}

void Buffer::loadShader(const std::string& vertexShader, const std::string& fragmentShader)
{

    _programID = glCreateProgram();
    int vertID = glCreateShader(GL_VERTEX_SHADER);
    int fragID = glCreateShader(GL_FRAGMENT_SHADER);

    int vertexShaderLength = vertexShader.length();
    auto vertexChar = vertexShader.c_str();
    glShaderSource(vertID, 1, &vertexChar, &vertexShaderLength);

    int fragmentShaderLength = fragmentShader.length();
    auto fragmentChar = fragmentShader.c_str();
    glShaderSource(fragID, 1, &fragmentChar, &fragmentShaderLength);

    glCompileShader(vertID);
    GLint param;
    glGetShaderiv(vertID, GL_COMPILE_STATUS, &param);
    if (param == GL_FALSE) {
        std::cout << "Failed to compile vertex shader!\n";
        int len;
        char log[200];
        glGetShaderInfoLog(fragID, 200, &len, log);
        std::cout << log;
    }

    glCompileShader(fragID);

    glGetShaderiv(fragID, GL_COMPILE_STATUS, &param);
    if (param == GL_FALSE) {
        std::cout << "Failed to compile fragment shader!\n";
        int len;
        char log[200];
        glGetShaderInfoLog(fragID, 200, &len, log);
        std::cout << log;
    }

    glAttachShader(_programID, vertID);
    glAttachShader(_programID, fragID);
    glLinkProgram(_programID);

    glValidateProgram(_programID);
    glGetProgramiv(_programID, GL_VALIDATE_STATUS, &param);
    if (param == GL_FALSE) {
        std::cout << "Shader program is not valid!\n";
        int len;
        char log[200];
        glGetShaderInfoLog(fragID, 200, &len, log);
        std::cout << log;
    }

    //Delete the shaders
    glDetachShader(_programID, vertID);
    glDetachShader(_programID, fragID);
    glDeleteShader(vertID);
    glDeleteShader(fragID);
}

GLuint Buffer::getProgramID() const
{
    return this->_programID;
}

} //namespace ojgl