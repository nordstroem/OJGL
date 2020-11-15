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
    GLState(const Window& window, const Demo& demo);
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

    Scene& operator[](size_t i) const;
    Scene& operator[](const ojstd::string& name) const;

private:
    void render();

    ojstd::vector<Scene> _scenes;
    Timepoint _systemClockStartTime;
    Duration _pauseTime;
    bool _paused = false;
    ojstd::shared_ptr<Music> _music = nullptr;
    Clock _clock = Clock::System;
    ojstd::shared_ptr<Buffer> _mainBuffer;
};

} // namespace ojgl
