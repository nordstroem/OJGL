#include "GLState.h"
#include "Uniform.hpp"
#include "music/Music.h"
#include "utility/Log.h"
#include "utility/ShaderReader.h"
#include "utility/Timepoint.h"
#include "winapi/gl_loader.h"

#ifdef _DEBUG
#include "thirdparty/stb_image_write.h"
#endif

namespace ojgl {

static ojstd::shared_ptr<Buffer> buildPassthroughBuffer(const Vector2i& windowSize, const Vector2i& sceneSize, bool isOutputBuffer)
{
    auto buffer = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "common/passThrough.fs");
    buffer->setViewportOffset((windowSize - sceneSize) / 2);
    buffer->generateFBO(isOutputBuffer); //@todo: make it possible to remove this line.
    return buffer;
}

GLState::GLState(const Window& window, const Demo& demo, bool saveFrames)
    : _saveFrames(saveFrames)
{
    const Vector2i sceneSize = window.size().cropToAspectRatio(demo.getAspectRatio());
    _mainBuffer = buildPassthroughBuffer(window.size(), sceneSize, true);
    _scenes = demo.buildSceneGraph(sceneSize);

    if (const auto* song = demo.getSong()) {
        Music::createInstance(song, saveFrames);
        Music::instance()->play();
        _clock = Clock::Music;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    _systemClockStartTime = Timepoint::now();

    if (saveFrames) {
        _outputImageData = static_cast<unsigned char*>(malloc(3 * _mainBuffer->size().x * _mainBuffer->size().y)); // RGB
        _saveBuffer = buildPassthroughBuffer(window.size(), sceneSize, false);
    }
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

#ifdef _DEBUG
    if (_saveFrames) {
        t = Duration::milliseconds(0);
        for (auto& v : _scenes) {
            if (elapsed < v.duration() + t) {
                _saveBuffer->setInputs(v.outputBuffer());
                _saveBuffer->render(sceneTime, elapsedSeconds);
                break;
            }
            t = t + v.duration();
        }

        const int w = _saveBuffer->size().x;
        const int h = _saveBuffer->size().y;
        glBindFramebuffer(GL_FRAMEBUFFER, _saveBuffer->currentFBO().fboID());
        glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, _outputImageData);
        for (int y = 0; y < h / 2; y++) {
            for (int x = 0; x < w; x++) {
                const unsigned char r = _outputImageData[y * w * 3 + 3 * x];
                const unsigned char g = _outputImageData[y * w * 3 + 3 * x + 1];
                const unsigned char b = _outputImageData[y * w * 3 + 3 * x + 2];

                _outputImageData[y * w * 3 + 3 * x] = _outputImageData[(h - y - 1) * w * 3 + 3 * x];
                _outputImageData[y * w * 3 + 3 * x + 1] = _outputImageData[(h - y - 1) * w * 3 + 3 * x + 1];
                _outputImageData[y * w * 3 + 3 * x + 2] = _outputImageData[(h - y - 1) * w * 3 + 3 * x + 2];

                _outputImageData[(h - y - 1) * w * 3 + 3 * x] = r;
                _outputImageData[(h - y - 1) * w * 3 + 3 * x + 1] = g;
                _outputImageData[(h - y - 1) * w * 3 + 3 * x + 2] = b;
            }
        }
        const int padding = 4 - (_currentFrame == 0 ? 0 : static_cast<int>(std::log10(_currentFrame)));

        ojstd::string padString = "";
        for (int i = 0; i < padding; i++) {
            padString = padString + "0";
        }
        ojstd::string x = ojstd::string("output_images/img") + padString + ojstd::to_string(_currentFrame) + ojstd::string(".tga");
        const int ret = stbi_write_tga(x.c_str(), w, h, 3, _outputImageData);
        _ASSERTE(ret);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
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
