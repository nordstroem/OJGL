
#include "gl_state.h"
#include "timer.h"
#include "window.h"
#include <functional>
#include <iostream>
#include <memory>
#include <stdio.h>
#include <string>
#include <thread>

using namespace std;

int main()
{
    bool fullScreen = false;
    Window window(fullScreen);
    GLState glState;
    while (true) {
        cout << Timer::time<Timer::ns_t>([&]() {
            window.getMessages();
            glState.render();
        }) << '\n';
    }

    return 0;
}