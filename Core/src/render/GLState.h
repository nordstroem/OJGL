#pragma once

#include "Scene.h"
#include "utility/Timer.hpp"
#include "winapi/gl_loader.h"
#include <functional>
#include <string>
#include <vector>

namespace ojgl {

class GLState {
public:
    GLState();
    ~GLState();

    void render();
    void addScene(const Scene& scene);
    void setStartTime(timer::Timepoint time);
    void restart();
    void nextScene();
    void previousScene();
    void togglePause();
    bool isPaused();
    void clearScenes();

    timer::Timepoint startTime() const;
    timer::Milliseconds relativeSceneTime() const;
    timer::Milliseconds elapsedTime() const;

    Scene& operator[](size_t i);
    Scene& operator[](const std::string& name);

private:
    void setupQuad();

    std::vector<Scene> _scenes;
    timer::Timepoint _startTime;
    timer::Timepoint _pauseTime;
    GLuint _vaoID;
    GLuint _vboID;
    bool _paused;

public:
    template <typename T>
    void changeTime(timer::Duration<T> time);
};

template <typename T>
inline void GLState::changeTime(timer::Duration<T> time)
{
    _startTime -= time;
}

} // namespace ojgl
