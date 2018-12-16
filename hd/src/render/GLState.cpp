#include "GLState.h"
#include "Uniform.hpp"
//#include "music/Music.h"
//#include "utility\Timer.hpp"

namespace ojgl {

GLState::GLState()
    : _startTime(Timepoint::now())
    , _pauseTime(Timepoint::now())
    , _paused(false)
{
    load_gl_functions();
    setupQuad();
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

void GLState::setStartTime(Timepoint time)
{
    _startTime = time;
}

void GLState::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(_vaoID);

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

Scene& GLState::operator[](size_t i)
{
    return _scenes[i];
}

Scene& GLState::operator[](const fl::string& name)
{
    auto res = fl::find_if(_scenes.begin(), _scenes.end(), [&](const auto& s) { return s.name() == name; });
    _ASSERTE(res != _scenes.end());
    return *res;
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
    auto elapsed = Timepoint::now() - _startTime;
    if (_paused) {
        elapsed = _pauseTime - _startTime;
    }
    return elapsed;
}

bool GLState::isPaused()
{
    return _paused;
}

void GLState::clearScenes()
{
    _scenes.clear();
}

Timepoint GLState::startTime() const
{
    return this->_startTime;
}

void GLState::togglePause()
{
    if (_paused) {
        _startTime += Timepoint::now() - _pauseTime;
    }
    _paused = !_paused;
    _pauseTime = Timepoint::now();
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
    _startTime = Timepoint::now();
    _pauseTime = _startTime;
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
