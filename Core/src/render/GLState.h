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
    void setStartTime(Timepoint time);
    void restart();
    void nextScene();
    void previousScene();
    void togglePause();
    bool isPaused();
    void clearScenes();

    Timepoint startTime() const;
    Duration relativeSceneTime() const;
    Duration elapsedTime() const;

    Scene& operator[](size_t i);
    Scene& operator[](const std::string& name);

private:
    void setupQuad();

    std::vector<Scene> _scenes;
    Timepoint _startTime;
    Timepoint _pauseTime;
    GLuint _vaoID;
    GLuint _vboID;
    bool _paused;

public:
    void changeTime(Duration time);
};

inline void GLState::changeTime(Duration time)
{
    _startTime -= time;
}

} // namespace ojgl
