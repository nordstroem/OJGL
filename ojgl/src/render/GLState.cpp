#include "GLState.h"
#include "Uniform.hpp"
#include "music/Music.h"
#include "utility/Log.h"
#include "utility/ShaderReader.h"
#include "utility/Timepoint.h"
#include "utility/stb_image_write.h"
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

    if (const auto* song = demo.getSong()) {
        Music::createInstance(song);
        Music::instance()->play();
        _clock = Clock::Music;
    }

    _clock = Clock::FixedTimestep;
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    _systemClockStartTime = Timepoint::now();

    _outputImageData = (uint8_t*)malloc(3 * window.size().x * window.size().y); // RGB
    _outputImageData2 = (uint8_t*)malloc(3 * window.size().x * window.size().y); // RGB
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
    const float elapsedSeconds = elapsed.toSeconds<float>();
    const float sceneTime = relativeSceneTime().toSeconds<float>();
    for (auto& v : _scenes) {
        if (elapsed < v.duration() + t) {
            v.render(sceneTime, elapsedSeconds);
            _mainBuffer->setInputs(v.outputBuffer());
            _mainBuffer->render(sceneTime, elapsedSeconds);
            break;
        }
        t = t + v.duration();
    }

    glFlush();
    glFinish();

    if (true || _currentFrame == 0) {
        auto w1 = Timepoint::now();
        constexpr int w = 1920;
        constexpr int h = 1080;
        glReadPixels(0, 0, 1920, 1080, GL_RGB, GL_UNSIGNED_BYTE, _outputImageData);
        auto w2 = Timepoint::now();
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                _outputImageData2[y * w * 3 + 3 * x] = _outputImageData[(h - y - 1) * w * 3 + 3 * x];
                _outputImageData2[y * w * 3 + 3 * x + 1] = _outputImageData[(h - y - 1) * w * 3 + 3 * x + 1];
                _outputImageData2[y * w * 3 + 3 * x + 2] = _outputImageData[(h - y - 1) * w * 3 + 3 * x + 2];
            }
        }
        auto w3 = Timepoint::now();

        int padding = 0;
        if (_currentFrame < 10)
            padding = 4;
        else if (_currentFrame < 100)
            padding = 3;
        else if (_currentFrame < 1000)
            padding = 2;
        else if (_currentFrame < 10000)
            padding = 1;

        ojstd::string padString = "";
        for (int i = 0; i < padding; i++) {
            padString = padString + "0";
        }
        ojstd::string x = ojstd::string("output_images/img") + padString + ojstd::to_string(_currentFrame) + ojstd::string(".png");
        stbi_write_png(x.c_str(), 1920, 1080, 3, _outputImageData2, 1920 * 3);
        auto w4 = Timepoint::now();
    }
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
    if (!this->isPaused()) {
        if (Music::instance() != nullptr)
            Music::instance()->updateSync();
        _currentFrame++;
    }
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
    } else if (_clock == Clock::Music) {
        // @todo assert that the DirectSound is active.
        return Music::instance()->elapsedTime();
    } else {
        return Duration(1000 * _currentFrame / 60);
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
    if (Music::instance() != nullptr)
        Music::instance()->setTime(time);
    _pauseTime = time;
}

void GLState::togglePause()
{
    if (!_paused) {
        _pauseTime = this->elapsedTime();
        if (Music::instance() != nullptr)
            Music::instance()->stop();
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
    if (Music::instance() != nullptr)
        Music::instance()->setTime(Duration::milliseconds(0));
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
