
//#include "../src/OJGL.h"
//#include "utility\ShaderReader.h"
//#include <fstream>
//#include <functional>
//#include <iostream>
//#include <memory>
//#include <set>
//#include <sstream>
//#include <streambuf>
//#include <string>
//#include <thread>
//#include <unordered_map>
//

#include "../fakelibs/fakelib.h"
#include "../src/render/GLState.h"
#include "../src/render/Texture.h"
#include "../src/render/Window.h"
#include "../src/utility/ShaderReader.h"
#include "EmbeddedResources.h"
#include <Windows.h>
using namespace ojgl;

void buildSceneGraph(GLState& glState, int x, int y)
{
    glState.clearScenes();

    {
        auto test = Buffer::construct(x, y, "intro2", "shaders/test.vs", "shaders/test2.fs");
        glState.addScene("test2", test, Duration::seconds(1));
    }

    {
        auto test = Buffer::construct(x, y, "intro", "shaders/test.vs", "shaders/test.fs");
        glState.addScene("test", test, Duration::seconds(50));
    }

    /*{
        auto edison = Buffer::construct(x, y, "intro", "shaders/edison.vs", "done/lavaIntro.fs");
        auto fxaa = Buffer::construct(x, y, "fxaa", "shaders/fxaa.vs", "shaders/fxaa.fs", edison);
        auto post = Buffer::construct(x, y, "post", "shaders/post.vs", "shaders/post.fs", fxaa);
        glState.addScene("introScene", post, Duration::seconds(22));
    }
    {
        auto noise = Buffer::construct(x, y, "intro", "shaders/demo.vs", "done/mountainNoise.fs");
        auto mountain = Buffer::construct(x, y, "fxaa", "shaders/demo.vs", "done/mountain.fs", noise);
        auto fxaa = Buffer::construct(x, y, "fxaa", "shaders/fxaa.vs", "shaders/fxaa.fs", mountain);
        auto post = Buffer::construct(x, y, "post", "shaders/demo.vs", "done/mountainPost.fs", fxaa);
        glState.addScene("introScene", post, Duration::seconds(77));
    }

    {
        auto edison = Buffer::construct(x, y, "intro", "shaders/edison.vs", "done/lavaScene2.fs");
        auto fxaa = Buffer::construct(x, y, "fxaa", "shaders/fxaa.vs", "shaders/fxaa.fs", edison);
        auto post = Buffer::construct(x, y, "post", "shaders/post.vs", "shaders/post.fs", fxaa);
        glState.addScene("introScene", post, Duration::seconds(40));
    }
    {
        auto edison = Buffer::construct(x, y, "intro", "shaders/edison.vs", "done/outro.fs");
        auto fxaa = Buffer::construct(x, y, "fxaa", "shaders/fxaa.vs", "shaders/fxaa.fs", edison);
        auto post = Buffer::construct(x, y, "post", "shaders/post.vs", "shaders/post.fs", fxaa);
        glState.addScene("introScene", post, Duration::seconds(40));
    }*/

    /*auto edison = Buffer::construct(x, y, "intro", "shaders/edison.vs", "done/outro.fs");
    auto fxaa = Buffer::construct(x, y, "fxaa", "shaders/fxaa.vs", "shaders/fxaa.fs", edison);
    auto post = Buffer::construct(x, y, "post", "shaders/post.vs", "shaders/post.fs", fxaa);
	glState.addScene("introScene", post, Duration::seconds(40));
	*/
}

int main(int argc, char* argv[])
{

    int width = 1280;
    int height = 720;
    bool full = true;

    //#ifdef _DEBUG
    //    full = false;
    //#endif
    //    if (argc >= 3) {
    //        width = std::stoi(argv[1]);
    //        height = std::stoi(argv[2]);
    //        if (argc >= 4) {
    //            full = static_cast<bool>(std::stoi(argv[3]));
    //        }
    //    }

    /* ShaderReader::setBasePath("examples/");

    ShaderReader::preLoad("shaders/edison.vs", resources::vertex::edison);
    ShaderReader::preLoad("shaders/demo.vs", resources::vertex::demo);
    ShaderReader::preLoad("shaders/post.vs", resources::vertex::post);
    ShaderReader::preLoad("shaders/fxaa.vs", resources::vertex::fxaa);
    ShaderReader::preLoad("shaders/fxaa.fs", resources::fragment::fxaa);
    ShaderReader::preLoad("shaders/post.fs", resources::fragment::post);
    ShaderReader::preLoad("done/lavaIntro.fs", resources::fragment::lavaIntro);
    ShaderReader::preLoad("done/mountain.fs", resources::fragment::mountain);
    ShaderReader::preLoad("done/mountainNoise.fs", resources::fragment::mountainNoise);
    ShaderReader::preLoad("done/mountainPost.fs", resources::fragment::mountainPost);
    ShaderReader::preLoad("done/lavaScene2.fs", resources::fragment::lavaScene2);
    ShaderReader::preLoad("done/outro.fs", resources::fragment::outro);*/

    ShaderReader::setBasePath("examples/");
    ShaderReader::preLoad("shaders/test.vs", resources::vertex::test);
    ShaderReader::preLoad("shaders/test.fs", resources::fragment::test);
    ShaderReader::preLoad("shaders/test2.fs", resources::fragment::test2);

    Window window(width, height, false);
    GLState glState;
    buildSceneGraph(glState, width, height);
    while (!glState.end()) {
        Timer timer;
        timer.start();
        glState.render();
        window.getMessages();

        glState << Uniform1f("iTime", glState.relativeSceneTime().toSeconds());

        timer.end();

        Sleep(16); // Are OpenGL calls async?
    }

    /* Music music(resources::songs::song);
    music.play();
    buildSceneGraph(glState, width, height);
    glState.setStartTime(Timepoint::now());

    auto previousPrintTime = Timepoint::now();
    while (!glState.end()) {
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
                glState.changeTime(Duration::milliseconds(-5000));
                timeChanged = true;
                break;

            case Window::KEY_RIGHT:
                glState.changeTime(Duration::milliseconds(5000));
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
#endif
            }

            if (!glState.isPaused() && timeChanged)
                music.setTime(glState.elapsedTime());
        }

        glState << Uniform1f("iTime", glState.relativeSceneTime().toSeconds());
        glState << Uniform1f("iGlobalTime", glState.relativeSceneTime().toSeconds() - 2.f);
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
    return 0;*/
}

extern "C" int _tmain(int argc, TCHAR** argv)
{
    return main(argc, argv);
}
