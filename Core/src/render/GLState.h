#pragma once

#include "winapi/gl_loader.h"
#include <string>

namespace ojgl {

class GLState {
public:
    GLState(const std::string& vertexShader, const std::string& fragmentShader);
    ~GLState();
    void render() const;
    GLuint getProgramID() const;
    GLuint getVAO() const;
    GLuint getVBO() const;

private:
    void loadShader(const std::string& vertexShader, const std::string& fragmentShader);
    void setupQuad();

    const unsigned _vertexCount = 6;
    DWORD _startTime;
    GLuint _programID;
    GLuint _vaoID;
    GLuint _vboID;
};
} // namespace ojgl