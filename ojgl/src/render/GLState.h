#pragma once

#include "Scene.h"
#include "Window.h"
#include "demo/Demo.h"
#include "utility/OJstd.h"
#include "utility/Timer.hpp"
#include "utility/Vector.h"

namespace ojgl {

enum class Clock {
    System,
    Music,
    FixedTimestep // 1/60 s per frame for now
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
    Clock _clock = Clock::System;
    ojstd::shared_ptr<Buffer> _mainBuffer;
    int _currentFrame = 0;
    mutable uint8_t* _outputImageData;
    mutable uint8_t* _outputImageData2;
};

} // namespace ojgl
