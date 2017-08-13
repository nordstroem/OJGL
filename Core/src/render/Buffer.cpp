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
    if (_fboID != 0)
        glDeleteFramebuffers(1, &_fboID);
    if (_fboTextureID != 0)
        glDeleteTextures(1, &_fboTextureID);

    glDeleteProgram(_programID);
}

std::shared_ptr<Buffer> Buffer::construct(const std::string& name, const std::string& vertex, const std::string& fragment)
{
    return construct(name, vertex, fragment, {});
}

std::shared_ptr<Buffer> Buffer::construct(const std::string& name, const std::string& vertex, const std::string& fragment, std::initializer_list<std::shared_ptr<Buffer>> buffers)
{
    return std::shared_ptr<Buffer>(new Buffer(name, vertex, fragment, buffers));
}

std::string Buffer::name()
{
    return _name;
}

unsigned Buffer::fboTextureID()
{
    return _fboTextureID;
}

void Buffer::render()
{
    glBindFramebuffer(GL_FRAMEBUFFER, _fboID);
    glViewport(0, 0, _width, _height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(_programID);
    for (size_t i = 0; i < _inputs.size(); i++)
        glUniform1i(glGetUniformLocation(_programID, "inTexture" + i), i);

    for (auto& um : _uniforms) {
        um.second->setUniform(_programID);
    }

    for (size_t i = 0; i < _inputs.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, _inputs[i]->fboTextureID());
    }

    glDrawArrays(GL_TRIANGLES, 0, GLState::vertexCount);

    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Buffer::generateFBO()
{
    glGenFramebuffers(1, &_fboID);
    glBindFramebuffer(GL_FRAMEBUFFER, _fboID);

    glGenTextures(1, &_fboTextureID);
    glBindTexture(GL_TEXTURE_2D, _fboTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _fboTextureID, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer error\n";

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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