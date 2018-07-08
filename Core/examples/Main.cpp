#include "EmbeddedResources.h"
#include "OJGL.h"
#include "utility\ShaderReader.h"
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <streambuf>
#include <string>
#include <thread>
#include <unordered_map>

using namespace ojgl;

void buildSceneGraph(GLState& glState, int x, int y)
{
    glState.clearScenes();

    auto edison = Buffer::construct(x, y, "intro", "edison.vs", "edison.fs");
    auto post = Buffer::construct(x, y, "post", "post.vs", "post.fs", edison);
    glState.addScene("introScene", post, Duration::maximum());
}

int main()
{
    int width = 1920 / 2;
    int height = 1080 / 2;
    ShaderReader::setBasePath("examples/shaders/");

    Window window(width, height, false);
    GLState glState;

    Music music(resources::songs::song);
    music.play();

    buildSceneGraph(glState, width, height);

    glState.setStartTime(Timepoint::now());

    auto previousPrintTime = Timepoint::now();
    while (true) {
        Timer timer;
        timer.start();
        window.getMessages();

        for (auto key : window.getPressedKeys()) {
            LOG_INFO("Key pressed: " << key);
            bool timeChanged = false;

            switch (key) {
            case Window::KEY_ESCAPE:
                return 0;
#ifdef _DEBUG
            case Window::KEY_LEFT:
                glState.changeTime(Duration::milliseconds(-1000));
                timeChanged = true;
                break;

            case Window::KEY_RIGHT:
                glState.changeTime(Duration::milliseconds(1000));
                timeChanged = true;
                break;

            case Window::KEY_SPACE:
                glState.togglePause();
                if (glState.isPaused())
                    music.stop();
                timeChanged = true;
                break;

            case Window::KEY_R:
                glState.restart();
                timeChanged = true;
                break;

            case Window::KEY_UP:
                glState.nextScene();
                timeChanged = true;
                break;

            case Window::KEY_DOWN:
                glState.previousScene();
                timeChanged = true;
                break;
            }

            if (!glState.isPaused() && timeChanged)
                music.setTime(glState.elapsedTime());
#endif
        }

        glState << Uniform1f("iTime", glState.relativeSceneTime().toSeconds());
        glState << Uniform2f("iResolution", static_cast<float>(width), static_cast<float>(height));

        glState.render();

        if (!glState.isPaused())
            music.updateSync();

        timer.end();

        using namespace std::chrono_literals;

        auto timeSinceLastPrint = Timepoint::now() - previousPrintTime;
        if (timeSinceLastPrint > Duration::seconds(2)) {
            LOG_INFO("Frame time: " << timer.currentTime());
            previousPrintTime = Timepoint::now();
        }
        std::this_thread::sleep_for(16ms);
    }
    return 0;
}
