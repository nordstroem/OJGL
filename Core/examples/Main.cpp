#include "OJGL.h"
#include "utility\Timer.hpp"
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdio.h>
#include <streambuf>
#include <string>
#include <thread>

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
            if (key == Window::KEY_ESCAPE) {
                return 0;
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
                std::ifstream shaderFile("examples/shaders/demo.fs");
                if (shaderFile.fail()) {
                    std::cout << "failed to open shader file\n";
                }
                std::stringstream buffer;
                buffer << shaderFile.rdbuf();
                std::string fileContents = buffer.str();
                size_t start = fileContents.find("R\"(");
                size_t end = fileContents.find_last_of(")\"");
                std::string shader = fileContents.substr(start + 3, end - start - 4);
                fragmentShader = shader;
                buildSceneGraph(glState);
            }
            if (!glState.isPaused() && timeChanged)
                music.setTime(glState.elapsedTime());
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
