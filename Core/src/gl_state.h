#pragma once

#include "winapi/gl_loader.h"

using namespace std;

class GLState {
public:
    GLState();
    ~GLState();
    void render() const;
    GLuint getProgramID() const;
    GLuint getVAO() const;
    GLuint getVBO() const;

private:
	const unsigned vertexCount = 6;
    void loadShader();
    void setupQuad();
    GLuint programID;
    GLuint vaoID;
    GLuint vboID;
};
