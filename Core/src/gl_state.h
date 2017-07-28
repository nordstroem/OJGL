#pragma once

#include "winapi/gl_loader.h"

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
	DWORD startTime;
    void loadShader();
    void setupQuad();
    GLuint programID;
    GLuint vaoID;
    GLuint vboID;
};
