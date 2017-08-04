#include "OJGL.h"
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
        window.getMessages();
        glState.render();
        mu.updateSync();
    }

    return 0;
}