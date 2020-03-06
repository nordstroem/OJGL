
#include "EmbeddedResources.h"
#include "render/GLState.h"
#include "render/Popup.h"
#include "render/Texture.h"
#include "render/Window.h"
#include "utility/Log.h"
#include "utility/Macros.h"
#include "utility/OJstd.h"
#include "utility/ShaderReader.h"

using namespace ojgl;

void buildSceneGraph(GLState& glState, int x, int y)
{
    glState.clearScenes();
    {
        auto buffer = Buffer::construct(x, y, "template", "common/quad.vs", "common/raymarch_base.fs", {}, ojgl::BufferFormat::Quad);
        glState.addScene("raymarch scene", buffer, Duration::seconds(100));
    }
}

int main(int argc, char* argv[])
{
    int width = 1280;
    int height = 720;
    bool fullScreen = false;
    bool showCursor = !fullScreen;
    ShaderReader::setBasePath("examples/shaders/");
    Window window(width, height, "Raymarch template", fullScreen, showCursor);
    GLState glState;
    buildSceneGraph(glState, width, height);
    glState.initialize();

    auto mesh = Mesh::constructCube();

    while (!glState.end() && !window.isClosePressed()) {
        Timer timer;
        timer.start();
        window.getMessages();

        for (auto key : window.getPressedKeys()) {
            switch (key) {
            case Window::KEY_ESCAPE:
                return 0;
#ifdef _DEBUG
            case Window::KEY_LEFT:
                glState.changeTime(Duration::milliseconds(-5000));
                break;
            case Window::KEY_RIGHT:
                glState.changeTime(Duration::milliseconds(5000));
                break;
            case Window::KEY_SPACE:
                glState.togglePause();
                break;
            case Window::KEY_R:
                glState.restart();
                break;
            case Window::KEY_UP:
                glState.nextScene();
                break;
            case Window::KEY_DOWN:
                glState.previousScene();
                break;
#endif
            }
        }

        glState << Uniform1f("iTime", glState.relativeSceneTime().toSeconds());
        glState << Uniform1f("iGlobalTime", glState.relativeSceneTime().toSeconds());
        glState << Uniform2f("iResolution", static_cast<float>(width), static_cast<float>(height));
        glState.update();

        timer.end();
        ojstd::sleep(10); // Are OpenGL calls async?
    }
}

extern "C" int _tmain(int argc, char** argv)
{
    return main(argc, argv);
}
