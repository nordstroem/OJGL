#pragma once

#include "../../fakelibs/fakelib.h"
#include "../utility/Timer.hpp"
#include "../winapi/gl_loader.h"
#include "Scene.h"

namespace ojgl {

class GLState {
public:
    GLState();
    ~GLState();

    void render();
    /*void setStartTime(Timepoint time);*/
    void restart();
    void nextScene();
    void previousScene();
    void togglePause();
    bool isPaused();
    void clearScenes();
    bool end();

    /*Timepoint startTime() const;
    Duration relativeSceneTime() const;
    Duration elapsedTime() const;*/

    Scene& operator[](size_t i);
    Scene& operator[](const fl::string& name);

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
            const auto buffers = scene.buffers();
            for (auto& buffer : buffers)
                *buffer << b;
        }
        return *this;
    }

private:
    void setupQuad();

    fl::vector<Scene> _scenes;
    /*Timepoint _startTime;
    Timepoint _pauseTime;*/
    GLuint _vaoID;
    GLuint _vboID;
    bool _paused;

public:
    /*void changeTime(Duration time);*/
};

//inline void GLState::changeTime(Duration time)
//{
//    _startTime -= time;
//}

} // namespace ojgl
