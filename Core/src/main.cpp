
#include "gl_state.h"
#include "window.h"
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
        window.getMessages();
        glState.render();
    }

    return 0;
}