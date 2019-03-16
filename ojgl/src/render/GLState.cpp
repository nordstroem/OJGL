#include "GLState.h"
#include "Uniform.hpp"
#include "utility/Log.h"
#include "utility/Timepoint.h"
#include "winapi/gl_loader.h"

namespace ojgl {

GLState::GLState()
    : _paused(false)
    , _clock(Clock::System)
{
    load_gl_functions();
    setupQuad();
}

GLState::GLState(unsigned char* song, Clock clock)
    : GLState()
{
    _clock = clock;
    _music = ojstd::make_shared<Music>(song);
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

GLState::~GLState()
{
    glDeleteVertexArrays(1, &_vaoID);
    glDeleteBuffers(1, &_vboID);
}

void GLState::initialize()
{
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
            break;
        }
        t = t + v.duration();
    }

    glBindVertexArray(0);
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
}

void GLState::setupQuad()
{
    float vertices[] = {
        -1, 1, 0, -1, -1, 0, 1, -1, 0,
        1, -1, 0, 1, 1, 0, -1, 1, 0
    };

    glGenVertexArrays(1, &_vaoID);
    glBindVertexArray(_vaoID);
    glEnableVertexAttribArray(0);
    glGenBuffers(1, &_vboID);
    glBindBuffer(GL_ARRAY_BUFFER, _vboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
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
} //namespace ojgl
