#include "GLState.h"
#include "Uniform.hpp"
#include "utility/Log.h"
#include "utility/ShaderReader.h"
#include "utility/Timepoint.h"
#include "winapi/gl_loader.h"

namespace ojgl {

static ojstd::shared_ptr<Buffer> buildPassthroughBuffer(const Vector2i& windowSize, const Vector2i& sceneSize)
{
    ojstd::string fragment {
#include "shaders/passThrough.fs"
    };
    ojstd::string vertex {
#include "shaders/passThrough.vs"
    };
    ShaderReader::preLoad("render/shaders/passThrough.fs", fragment);
    ShaderReader::preLoad("render/shaders/passThrough.vs", vertex);

    auto buffer = Buffer::construct(sceneSize.x, sceneSize.y, "render/shaders/passThrough.vs", "render/shaders/passThrough.fs");
    buffer->setViewportOffset((windowSize - sceneSize) / 2);
    buffer->generateFBO(true); //@todo: make it possible to remove this line.
    return buffer;
}

GLState::GLState(const Window& window, const Demo& demo)
{
    const Vector2i sceneSize = window.size().cropToAspectRatio(demo.getAspectRatio());
    _mainBuffer = buildPassthroughBuffer(window.size(), sceneSize);
    _scenes = demo.buildSceneGraph(sceneSize);

    if (auto* song = demo.getSong()) {
        _music = ojstd::make_shared<Music>(song);
        _music->play();
        _clock = Clock::Music;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    _systemClockStartTime = Timepoint::now();
}

bool GLState::end() const
{
    auto t = Duration::milliseconds(0);
    auto elapsed = elapsedTime();

    for (auto& v : _scenes) {
        if (elapsed < v.duration() + t) {
            return false;
        }
        t = t + v.duration();
    }
    return true;
}

void GLState::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto t = Duration::milliseconds(0);
    const auto elapsed = elapsedTime();
    const float sceneTime = relativeSceneTime().toSeconds<float>();
    for (auto& v : _scenes) {
        if (elapsed < v.duration() + t) {
            v.render(sceneTime);
            _mainBuffer->setInputs(v.outputBuffer());
            _mainBuffer->render(sceneTime);
            break;
        }
        t = t + v.duration();
    }

    glFlush();
    glFinish();
}

Music& GLState::music() const
{
    _ASSERTE(_music != nullptr);
    return *_music;
}

Scene& GLState::operator[](size_t i) const
{
    return _scenes[i];
}

Scene& GLState::operator[](const ojstd::string& name) const
{
    auto res = ojstd::find_if(_scenes.begin(), _scenes.end(), [&](const auto& s) { return s.name() == name; });
    _ASSERTE(res != _scenes.end());
    return *res;
}

void GLState::update()
{
    this->render();
    if (!this->isPaused())
        if (_music != nullptr)
            _music->updateSync();

    // Clear meshes
    for (auto& v : _scenes) {
        auto buffers = v.buffers();
        for (auto& b : buffers)
            b->clearMeshes();
    }
}

Duration GLState::elapsedTime() const
{
    if (_paused)
        return _pauseTime;
    if (_clock == Clock::System) {
        return Timepoint::now() - _systemClockStartTime;
    } else {
        // @todo assert that the DirectSound is active.
        return _music->elapsedTime();
    }
}

bool GLState::isPaused() const
{
    return _paused;
}

void GLState::changeTime(Duration time)
{
    this->setTime(this->elapsedTime() + time);
}

void GLState::setTime(Duration time)
{
    _systemClockStartTime = Timepoint::now() - time;
    if (_music != nullptr)
        _music->setTime(time);
    _pauseTime = time;
}

void GLState::togglePause()
{
    if (!_paused) {
        _pauseTime = this->elapsedTime();
        if (_music != nullptr)
            _music->stop();
    } else {
        this->setTime(_pauseTime);
    }
    _paused = !_paused;
}

Duration GLState::relativeSceneTime() const
{
    auto t = Duration::milliseconds(0);
    auto elapsed = elapsedTime();
    for (auto& v : _scenes) {
        if (elapsed < v.duration() + t) {
            break;
        }
        t = t + v.duration();
    }
    return elapsed - t;
}

void GLState::restart()
{
    _systemClockStartTime = Timepoint::now();
    _paused = false;
    if (_music != nullptr)
        _music->setTime(Duration::milliseconds(0));
}

void GLState::nextScene()
{
    auto t = Duration::milliseconds(0);
    auto elapsed = elapsedTime();
    for (auto& v : _scenes) {
        if (elapsed < v.duration() + t) {
            changeTime(v.duration() - relativeSceneTime());
            break;
        }
        t = t + v.duration();
    }
}

void GLState::previousScene()
{
    auto t = Duration::milliseconds(0);
    auto prevDur = Duration::milliseconds(0);
    auto elapsed = elapsedTime();
    for (auto& v : _scenes) {
        if (elapsed < v.duration() + t) {
            changeTime(-relativeSceneTime() - prevDur);
            break;
        }
        t = t + v.duration();
        prevDur = v.duration();
    }
}

ojstd::string GLState::currentScene() const
{
    auto t = Duration::milliseconds(0);
    auto elapsed = elapsedTime();
    for (auto& v : _scenes) {
        if (elapsed < v.duration() + t) {
            return v.name();
        }
        t = t + v.duration();
    }
    return "";
}

} //namespace ojgl
