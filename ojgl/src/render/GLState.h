#pragma once

#include "Scene.h"
#include "Window.h"
#include "demo/Demo.h"
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
    GLState(const Window& window, const Vector2i& sceneSize, unsigned char* song, const ojstd::shared_ptr<Demo>& demo, Clock clock = Clock::Music);
    GLState(const GLState& other) = delete;
    GLState& operator=(const GLState& other) = delete;

    void update();
    void changeTime(Duration time);
    void setTime(Duration time);
    void restart();
    void nextScene();
    void previousScene();
    void togglePause();
    bool isPaused() const;
    [[nodiscard]] bool end() const;
    ojstd::string currentScene() const;

    Music& music() const;
    Duration relativeSceneTime() const;
    Duration elapsedTime() const;

    Vector2i sceneSize() const;

    Scene& operator[](size_t i) const;
    Scene& operator[](const ojstd::string& name) const;

public:
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
    ojstd::shared_ptr<Demo> _demo;
};

} // namespace ojgl
