#include "GLState.h"
#include "Uniform.hpp"
#include "music/Music.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace ojgl {

GLState::GLState(const std::string& vertexShader, const std::string& fragmentShader)
{
    _startTime = GetTickCount();
    load_gl_functions();
    setupQuad();
}

GLState::~GLState()
{
    glDeleteVertexArrays(1, &_vaoID);
    glDeleteBuffers(1, &_vboID);
}

void GLState::addScene(const Scene& scene)
{
    _scenes.push_back(scene);
}

void GLState::render()
{
    /* glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(_programID);

    for (auto& um : _uniforms) {
        um.second->setUniform(_programID);
    }

    glBindVertexArray(_vaoID);
    glDrawArrays(GL_TRIANGLES, 0, _vertexCount);
    glBindVertexArray(0);
    glFlush();*/
}

void GLState::setupQuad()
{
    float vertices[] = {
        -1, 1, 0, -1, -1, 0, 1, -1, 0,
        1, -1, 0, 1, 1, 0, -1, 1, 0
    };

    glGenVertexArrays(1, &_vaoID);
    glBindVertexArray(_vaoID);
    glEnableVertexAttribArray(0);
    glGenBuffers(1, &_vboID);
    glBindBuffer(GL_ARRAY_BUFFER, _vboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

GLuint GLState::getVAO() const
{
    return this->_vaoID;
}

GLuint GLState::getVBO() const
{
    return this->_vboID;
}

DWORD GLState::startTime() const
{
    return this->_startTime;
}
}