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
    template <typename T>
    void changeTime(timer::duration_t<T>);
    void togglePause();
    timer::ms_t relativeSceneTime();
    void restart();
    void nextScene();
    void previousScene();
    timer::ms_t elapsedTime();
    bool isPaused();
    void clearScenes();
    static const unsigned vertexCount = 6;

private:
    void setupQuad();

    timer::time_point_t _startTime;
    GLuint _vaoID;
    GLuint _vboID;
    std::vector<Scene> _scenes;
    bool _paused;
    timer::time_point_t _pauseTime;
};

template <typename T>
inline void GLState::changeTime(timer::duration_t<T> time)
{
    _startTime -= time;
}

} // namespace ojgl