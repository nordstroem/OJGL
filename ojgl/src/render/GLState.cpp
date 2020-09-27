#include "GLState.h"
#include "Uniform.hpp"
#include "utility/Log.h"
#include "utility/ShaderReader.h"
#include "utility/Timepoint.h"
#include "winapi/gl_loader.h"

namespace ojgl {

static Vector2i cropToAspectRatio(const Vector2i& windowSize, float aspectRatio);

GLState::GLState(const Window& window, float sceneAspectRatio, unsigned char* song, Clock clock)
    : _paused(false)
    , _clock(clock)
    , _music(ojstd::make_shared<Music>(song))
    , _sceneSize(cropToAspectRatio(window.size(), sceneAspectRatio))
{
    load_gl_functions();asd

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
}

static Vector2i cropToAspectRatio(const Vector2i& windowSize, float aspectRatio)
{
    const float windowAspectRatio = static_cast<float>(windowSize.x) / windowSize.y;

    if (aspectRatio > windowAspectRatio) {
        return Vector2i(windowSize.x, ojstd::ftoi(windowSize.x / aspectRatio));
    } else {
        return Vector2i(ojstd::ftoi(windowSize.y * aspectRatio), windowSize.y);
    }
}

bool GLState::end()
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

void GLState::initialize()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    _systemClockStartTime = Timepoint::now();
    if (_music != nullptr)
        _music->play();
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

Music& GLState::music()
{
    _ASSERTE(_music != nullptr);
    return *_music;
}

Scene& GLState::operator[](size_t i)
{
    return _scenes[i];
}

Scene& GLState::operator[](const ojstd::string& name)
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

bool GLState::isPaused()
{
    return _paused;
}

void GLState::clearScenes()
{
    _scenes.clear();
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

Duration GLState::relativeSceneTime()
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

ojstd::string GLState::currentScene()
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
