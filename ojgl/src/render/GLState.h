﻿#pragma once

#include "Scene.h"
#include "Window.h"
#include "demo/Demo.h"
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
    GLState(const Window& window, const ojstd::shared_ptr<Demo>& demo, bool saveFrames);
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
    void saveFrame() const;
    ojstd::vector<Scene> _scenes;
    Timepoint _systemClockStartTime;
    Duration _pauseTime;
    bool _paused = false;
    Clock _clock = Clock::System;
    ojstd::shared_ptr<Buffer> _mainBuffer;
    ojstd::shared_ptr<Demo> _demo;
    int _currentFrame = 0;
    bool _saveFrames;

    mutable unsigned char* _outputImageData;
    ojstd::shared_ptr<Buffer> _saveBuffer;
};

} // namespace ojgl
