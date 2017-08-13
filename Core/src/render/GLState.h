#pragma once

#include "Scene.h"
#include "winapi/gl_loader.h"
#include <functional>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>

namespace ojgl {

class GLState {
public:
    GLState();
    ~GLState();
    void render();
    void addScene(const Scene& scene);
    Scene& operator[](size_t i);
    GLuint getVAO() const;
    GLuint getVBO() const;
    DWORD startTime() const;

    static const unsigned vertexCount = 6;

private:
    void setupQuad();

    DWORD _startTime;
    GLuint _vaoID;
    GLuint _vboID;
    std::vector<Scene> _scenes;
};

} // namespace ojgl