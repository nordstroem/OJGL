#include "GLState.h"
#include "Uniform.hpp"
#include "utility/Log.h"
#include "utility/ShaderReader.h"
#include "utility/Timepoint.h"
#include "winapi/gl_loader.h"

namespace ojgl {

GLState::GLState(const Window& window, const Vector2i& sceneSize, unsigned char* song, ojstd::vector<Scene>&& scenes, Clock clock)
    : _scenes(std::move(scenes))
    , _paused(false)
    , _clock(clock)
    , _music(ojstd::make_shared<Music>(song))
    , _sceneSize(sceneSize)
{
    //load_gl_functions();

    ojstd::string fragment {
#include "shaders/passThrough.fs"
    };
    ojstd::string vertex {
#include "shaders/passThrough.vs"
    };
    ShaderReader::preLoad("render/shaders/passThrough.fs", fragment);
    ShaderReader::preLoad("render/shaders/passThrough.vs", vertex);
    _mainBuffer = Buffer::construct(_sceneSize.x, _sceneSize.y, "render/shaders/passThrough.vs", "render/shaders/passThrough.fs");
    _mainBuffer->setViewportOffset((window.size() - _sceneSize) / 2);
    _mainBuffer->generateFBO(true); //@todo: make it possible to remove this line.

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    _systemClockStartTime = Timepoint::now();
    if (_music != nullptr)
        _music->play();
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
    auto elapsed = elapsedTime();
    for (auto& v : _scenes) {
        if (elapsed < v.duration() + t) {
            v.render();
            _mainBuffer->setInputs(v.outputBuffer());
            _mainBuffer->render();
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

Vector2i GLState::sceneSize() const
{
    return _sceneSize;
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
