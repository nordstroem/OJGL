#include "OJGL.h"
#include "utility\Timer.hpp"
#include <functional>
#include <memory>
#include <stdio.h>
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

int main()
{
    const double desiredFrameTimeMs = 1000.0 / 60.0;
    Window window(1024, 768, false);
    GLState glState;
    Music music(song);
    music.play();

    auto pre = Buffer::construct(1024, 768, "main", vertexShader, fragmentShader);
    auto post = Buffer::construct(1024, 768, "post", vertexShaderPost, fragmentShaderPost);

    Scene scene{ pre, timer::ms_t(1000) };
    Scene scene2{ post, timer::ms_t(1000) };

    glState.addScene(scene);
    glState.addScene(scene2);
    glState.addScene(scene);
    glState.addScene(scene2);
    glState.addScene(scene);
    glState.addScene(scene2);

    glState.setStartTime(timer::clock_t::now());

    while (true) {
        timer::Timer t;
        t.start();

        window.getMessages();

        glState[0]["post"] << Uniform1f("r", 0.9f);

        auto iGlobalTime = timer::clock_t::now() - glState.startTime();
        glState[0]["main"] << Uniform1f("iGlobalTime", iGlobalTime.count() / 1000.f)
                           << Uniform1f("CHANNEL_12_TOTAL", (float)music.syncChannels[12].getTotalHitsPerNote(0))
                           << Uniform1f("CHANNEL_13_TOTAL", (float)music.syncChannels[13].getTotalHitsPerNote(0));

        for (auto& kv : music.syncChannels) {
            auto sc = kv.second;
            std::vector<GLfloat> valuesSince;
            std::vector<GLfloat> valuesTo;

            for (int i = 0; i < sc.numNotes; i++) {
                valuesSince.push_back((GLfloat)sc.getTimeSinceLast(i));
                valuesTo.push_back((GLfloat)sc.getTimeToNext(i));
            }

            glState[0]["main"] << Uniform1fv("CHANNEL_" + std::to_string(sc.channel) + "_TIME_SINCE", valuesSince)
                               << Uniform1fv("CHANNEL_" + std::to_string(sc.channel) + "_TIME_TO", valuesTo);
        }

        glState.render();
        music.updateSync();
        t.end();
        auto durationMs = t.time<timer::ms_t>();
        if (durationMs < desiredFrameTimeMs) {
            std::this_thread::sleep_for(std::chrono::milliseconds((int)(desiredFrameTimeMs)-durationMs));
        }
    }

    return 0;
}
