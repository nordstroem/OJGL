#include "gl_state.h"
#include "util.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

GLState::GLState()
{
    startTime = GetTickCount();
    load_gl_functions();
    setupQuad();
    loadShader();
}

GLState::~GLState()
{
    glDeleteVertexArrays(1, &vaoID);
    glDeleteBuffers(1, &vboID);
    glDeleteProgram(programID);
}

void GLState::render() const
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(programID);
    glUniform1f(glGetUniformLocation(programID, "iGlobalTime"), (GetTickCount() - startTime) / 1000.0);
    glBindVertexArray(vaoID);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
    glFlush();
}

void GLState::setupQuad()
{
    float vertices[] = {
        -1, 1, 0, -1, -1, 0, 1, -1, 0,
        1, -1, 0, 1, 1, 0, -1, 1, 0
    };

    glGenVertexArrays(1, &vaoID);
    std::cout << vaoID << '\n';
    glBindVertexArray(vaoID);
    glEnableVertexAttribArray(0);
    glGenBuffers(1, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void GLState::loadShader()
{

    std::string vertexShader =
#include "../shaders/demo.vs"
        ;

    std::string fragmentShader =
#include "../shaders/demo.fs"
        ;

    programID = glCreateProgram();
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

    glAttachShader(programID, vertID);
    glAttachShader(programID, fragID);
    glLinkProgram(programID);

    glValidateProgram(programID);
    glGetProgramiv(programID, GL_VALIDATE_STATUS, &param);
    if (param == GL_FALSE) {
        std::cout << "Shader program is not valid!\n";
        int len;
        char log[200];
        glGetShaderInfoLog(fragID, 200, &len, log);
        std::cout << log;
    }

    //Delete the shaders
    glDetachShader(programID, vertID);
    glDetachShader(programID, fragID);
    glDeleteShader(vertID);
    glDeleteShader(fragID);
}

GLuint GLState::getProgramID() const
{
    return this->programID;
}

GLuint GLState::getVAO() const
{
    return this->vaoID;
}

GLuint GLState::getVBO() const
{
    return this->vboID;
}