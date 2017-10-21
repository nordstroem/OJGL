#define TINYDEFLATE_IMPL

#include "OJGL.h"
#include "utility\Timer.hpp"
#include "winapi\tinydeflate.h"
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <stdio.h>
#include <streambuf>
#include <string>
#include <thread>

#define DEBUG

unsigned char song[] = {
#include "songs/song.inc"
};

std::string vertexShader{
#include "shaders/demo.vs"
};

std::string fragmentShader{
#include "shaders/demo.fs"
};

std::string vertexShaderPost{
#include "shaders/post.vs"
};

std::string fragmentShaderPost{
#include "shaders/post.fs"
};

using namespace ojgl;

#ifdef DEBUG
void reloadShaders()
{
    std::map<std::string*, std::string> shaders;
    shaders[&fragmentShader] = std::string("examples/shaders/demo.fs");
    shaders[&fragmentShaderPost] = std::string("examples/shaders/post.fs");

    for (auto[stringptr, path] : shaders) {
        std::ifstream shaderFile(path);
        if (shaderFile.fail()) {
            std::cout << "failed to open shader file\n";
        }
        std::stringstream buffer;
        buffer << shaderFile.rdbuf();
        std::string fileContents = buffer.str();
        std::string pre = "R\"(";
        std::string post = ")\"";
        size_t start = fileContents.find(pre);
        size_t end = fileContents.find_last_of(post);
        std::string shader = fileContents.substr(start + pre.length(), end - start - post.length() - 2);
        *stringptr = shader;
    }
}
#endif

void buildSceneGraph(GLState& glState)
{
    glState.clearScenes();

    auto pre = Buffer::construct(1024, 768, "main", vertexShader, fragmentShader);
    auto post = Buffer::construct(1024, 768, "post", vertexShaderPost, fragmentShaderPost);

    Scene scene{ pre, timer::ms_t(30000) };
    Scene scene2{ post, timer::ms_t(5000) };

    glState.addScene(scene);
    glState.addScene(scene2);
    glState.addScene(scene);
    glState.addScene(scene2);
    glState.addScene(scene);
    glState.addScene(scene2);
}

int main()
{
    tdImage img = tdLoadPNG("images/fredag.png");
    Texture texture(img.w, img.h, reinterpret_cast<unsigned char*>(img.pix));

    const double desiredFrameTimeMs = 1000.0 / 60.0;
    Window window(1024, 768, false);
    GLState glState;

    Music music(song);
    music.play();

    buildSceneGraph(glState);

    glState.setStartTime(timer::clock_t::now());

    while (true) {
        timer::Timer t;
        t.start();

        window.getMessages();

        for (auto key : window.getPressedKeys()) {
            if (key == Window::KEY_ESCAPE) {
                return 0;
            }
#ifdef DEBUG
            bool timeChanged(false);
            std::cout << "key: " << key << "\n";
            if (key == Window::KEY_LEFT) {
                glState.changeTime(timer::ms_t(-1000));
                timeChanged = true;
            }
            if (key == Window::KEY_RIGHT) {
                glState.changeTime(timer::ms_t(1000));
                timeChanged = true;
            }
            if (key == Window::KEY_SPACE) {
                glState.togglePause();
                if (glState.isPaused())
                    music.stop();
                timeChanged = true;
            }

            if (key == Window::KEY_R) {
                glState.restart();
                timeChanged = true;
            }
            if (key == Window::KEY_UP) {
                glState.nextScene();
                timeChanged = true;
            }
            if (key == Window::KEY_DOWN) {
                glState.previousScene();
                timeChanged = true;
            }
            if (key == Window::KEY_F1) {
                reloadShaders();
                buildSceneGraph(glState);
            }

            if (!glState.isPaused() && timeChanged)
                music.setTime(glState.elapsedTime());
#endif
        }

        glState[0]["post"] << Uniform1f("r", 0.9f);

        auto iGlobalTime = glState.relativeSceneTime();
        glState[0]["main"] << Uniform1f("iGlobalTime", iGlobalTime.count() / 1000.f)
                           << Uniform1f("CHANNEL_12_TOTAL", static_cast<GLfloat>(music.syncChannels[12].getTotalHitsPerNote(0)))
                           << Uniform1f("CHANNEL_13_TOTAL", static_cast<GLfloat>(music.syncChannels[13].getTotalHitsPerNote(0)));

        for (auto& kv : music.syncChannels) {
            auto sc = kv.second;
            std::vector<GLfloat> valuesSince;
            std::vector<GLfloat> valuesTo;

            for (int i = 0; i < sc.numNotes; i++) {
                valuesSince.push_back(static_cast<GLfloat>(sc.getTimeSinceLast(i).count()));
                valuesTo.push_back(static_cast<GLfloat>(sc.getTimeToNext(i).count()));
            }

            glState[0]["main"] << Uniform1fv("CHANNEL_" + std::to_string(sc.channel) + "_TIME_SINCE", valuesSince)
                               << Uniform1fv("CHANNEL_" + std::to_string(sc.channel) + "_TIME_TO", valuesTo);
        }

        glState.render();
        if (!glState.isPaused())
            music.updateSync();
        t.end();
        auto durationMs = t.time<timer::ms_t>();
        if (durationMs < desiredFrameTimeMs) {
            std::this_thread::sleep_for(std::chrono::milliseconds((int)(desiredFrameTimeMs)-durationMs));
        }
    }

    return 0;
}
