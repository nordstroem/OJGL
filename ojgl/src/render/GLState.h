#pragma once

#include "Scene.h"
#include "Window.h"
#include "music/Music.h"
#include "utility/OJstd.h"
#include "utility/Timer.hpp"
#include "utility/Vector.h"

namespace ojgl {

enum class Clock {
    System,
    Music
};

class GLState {
public:
    GLState(const Window& window, float sceneAspectRatio, unsigned char* song, Clock clock = Clock::Music);
    GLState(const GLState& other) = delete;
    GLState& operator=(const GLState& other) = delete;

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
    ojstd::string currentScene();

    Music& music();
    Duration relativeSceneTime();
    Duration elapsedTime() const;

    Vector2i sceneSize() const;

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
    void render();

    ojstd::vector<Scene> _scenes;
    Timepoint _systemClockStartTime;
    Duration _pauseTime;
    bool _paused;
    ojstd::shared_ptr<Music> _music = nullptr;
    Clock _clock = Clock::System;
    ojstd::shared_ptr<Buffer> _mainBuffer;
    Vector2i _sceneSize;
};

} // namespace ojgl
