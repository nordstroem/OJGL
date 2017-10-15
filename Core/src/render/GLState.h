#pragma once

#include "Scene.h"
#include "utility/Timer.hpp"
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
    void setStartTime(timer::time_point_t);
    Scene& operator[](size_t i);
    GLuint getVAO() const;
    GLuint getVBO() const;
    timer::time_point_t startTime() const;

    static const unsigned vertexCount = 6;

private:
    void setupQuad();

    timer::time_point_t _startTime;
    GLuint _vaoID;
    GLuint _vboID;
    std::vector<Scene> _scenes;
};

} // namespace ojgl