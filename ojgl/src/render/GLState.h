#pragma once

#include "Scene.h"
#include "music/Music.h"
#include "utility/OJstd.h"
#include "utility/Timer.hpp"

namespace ojgl {

enum class Clock {
    System,
    Music
};

class GLState {
public:
    GLState();
    explicit GLState(unsigned char* song, Clock clock = Clock::Music);
    GLState(const GLState& other) = delete;
    GLState& operator=(const GLState& other) = delete;
    ~GLState();

    void initialize();
    void update();
    void changeTime(Duration time);
    void setTime(Duration time);
    void restart();
    void nextScene();
    void previousScene();
    void togglePause();
    bool isPaused();
    void clearScenes();
    bool end();

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
    void render();

    ojstd::vector<Scene> _scenes;
    Timepoint _systemClockStartTime;
    Duration _pauseTime;
    unsigned int _vaoID;
    unsigned int _vboID;
    bool _paused;
    Music _music;
    Clock _clock = Clock::System;
};

} // namespace ojgl
