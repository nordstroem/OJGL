
#include "gl_state.h"
#include "music\music.h"
#include "timer.h"
#include "window.h"
#include <functional>
#include <memory>
#include <stdio.h>
#include <string>
#include <thread>

using namespace std;

int main()
{
    bool fullScreen = false;

    Music mu;
    mu.play();

    Window window(fullScreen);
    GLState glState;

    while (true) {
        //cout << Timer::time<Timer::ms_t>([&]() {
        window.getMessages();
        glState.render();
        mu.updateSync();
        //}) << '\n';
    }

    return 0;
}