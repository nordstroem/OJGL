#include "Buffer.h"
#include "GLState.h"
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
    glDeleteProgram(_programID);
}

std::shared_ptr<Buffer> Buffer::construct(const std::string& name, const std::string& vertex, const std::string& fragment)
{
    return construct(name, fragment, vertex, {});
}

std::shared_ptr<Buffer> Buffer::construct(const std::string& name, const std::string& vertex, const std::string& fragment, std::initializer_list<std::shared_ptr<Buffer>> buffers)
{
    return std::shared_ptr<Buffer>(new Buffer(name, fragment, vertex, buffers));
}

bool Buffer::hasRun()
{
    return _hasRun;
}

void Buffer::reset()
{
    _hasRun = false;
    for (auto& b : _inputs) {
        if (b->_hasRun) {
            b->reset();
        }
    }
}

void Buffer::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(_programID);

    for (auto& um : _uniforms) {
        um.second->setUniform(_programID);
    }

    glDrawArrays(GL_TRIANGLES, 0, GLState::vertexCount);
    glFlush();

    _hasRun = true;
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