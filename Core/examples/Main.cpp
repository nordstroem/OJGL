#include "OJGL.h"
#include "utility\Timer.hpp"
#include <chrono>
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

using namespace ojgl;

int main()
{

    Window window(false);
    GLState glState(vertexShader, fragmentShader);
    Music mu(song);

    mu.play();

    while (true) {
        timer::Timer t;
        t.start();
        window.getMessages();
        glState.render(mu);
        mu.updateSync();
        t.end();
        auto dur = t.time<timer::ms_t>();
        if (dur < 1000.0 / 60.0) {
            std::this_thread::sleep_for(std::chrono::milliseconds((int)(1000.0 / 60.0) - dur));
        }
    }

    return 0;
}