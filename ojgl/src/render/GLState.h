#pragma once

#include "Scene.h"
#include "utility/OJstd.h"
#include "utility/Timer.hpp"

namespace ojgl {

class GLState {
public:
    GLState();
    GLState(const GLState& other) = delete;
    GLState& operator=(const GLState& other) = delete;
    ~GLState();

    void render();
    void setStartTime(Timepoint time);
    void restart();
    void nextScene();
    void previousScene();
    void togglePause();
    bool isPaused();
    void clearScenes();
    bool end();

    Timepoint startTime() const;
    Duration relativeSceneTime();
    Duration elapsedTime() const;

    Scene& operator[](size_t i);
    Scene& operator[](const ojstd::string& name);

public:
    template <typename... Args>
    void addScene(Args&&... args)
    {
        this->_scenes.emplace_back(std::forward<Args>(args)...);
    }

    template <typename T>
    GLState& operator<<(T&& b)
    {
        for (auto& scene : this->_scenes) {
            auto buffers = scene.buffers();
            for (auto& buffer : buffers)
                *buffer << b;
        }
        return *this;
    }

private:
    void setupQuad();

    ojstd::vector<Scene> _scenes;
    Timepoint _startTime;
    Timepoint _pauseTime;
    unsigned int _vaoID;
    unsigned int _vboID;
    bool _paused;

public:
    void changeTime(Duration time);
};

inline void GLState::changeTime(Duration time)
{
    _startTime -= time;
}

} // namespace ojgl
