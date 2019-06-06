
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
        auto edison = Buffer::construct(BufferFormat::Quad, x, y, "intro", "shaders/edison.vs", "shaders/edison_1.fs");
        auto fxaa = Buffer::construct(BufferFormat::Quad, x, y, "fxaa", "shaders/fxaa.vs", "shaders/fxaa.fs", edison);
        auto post = Buffer::construct(BufferFormat::Quad, x, y, "post", "shaders/post.vs", "shaders/edison_1_post.fs", fxaa);

        //  auto mesh = Buffer::construct(BufferFormat::Meshes, x, y, "mesh", "shaders/mesh.vs", "shaders/mesh.fs");

        glState.addScene("meshScene", post, Duration::seconds(1202));
    }
    {
        auto noise = Buffer::construct(BufferFormat::Quad, x, y, "intro", "shaders/demo.vs", "shaders/mountainNoise.fs");
        auto mountain = Buffer::construct(BufferFormat::Quad, x, y, "fxaa", "shaders/demo.vs", "shaders/mountain.fs", noise);
        auto fxaa = Buffer::construct(BufferFormat::Quad, x, y, "fxaa", "shaders/fxaa.vs", "shaders/fxaa.fs", mountain);
        auto post = Buffer::construct(BufferFormat::Quad, x, y, "post", "shaders/demo.vs", "shaders/mountainPost.fs", fxaa);
        glState.addScene("introScene", post, Duration::seconds(77));
    }

    {
        auto edison = Buffer::construct(BufferFormat::Quad, x, y, "intro", "shaders/edison.vs", "shaders/lavaScene2.fs");
        auto fxaa = Buffer::construct(BufferFormat::Quad, x, y, "fxaa", "shaders/fxaa.vs", "shaders/fxaa.fs", edison);
        auto post = Buffer::construct(BufferFormat::Quad, x, y, "post", "shaders/post.vs", "shaders/post.fs", fxaa);
        glState.addScene("introScene", post, Duration::seconds(40));
    }
    {
        auto edison = Buffer::construct(BufferFormat::Quad, x, y, "intro", "shaders/edison.vs", "shaders/outro.fs");
        auto fxaa = Buffer::construct(BufferFormat::Quad, x, y, "fxaa", "shaders/fxaa.vs", "shaders/fxaa.fs", edison);
        auto post = Buffer::construct(BufferFormat::Quad, x, y, "post", "shaders/post.vs", "shaders/post.fs", fxaa);
        glState.addScene("introScene", post, Duration::seconds(40));
    }
}

struct Camera {
    float d1 = 0;
    float d2 = 0;
    float d3 = 0;

    void tick(int key)
    {
        auto isDown = [key](int keyCode) { return key == keyCode ? 1 : 0; };
        this->d1 += isDown(Window::KEY_Q);
        this->d1 -= isDown(Window::KEY_A);
        this->d2 += isDown(Window::KEY_W);
        this->d2 -= isDown(Window::KEY_S);
        this->d3 += isDown(Window::KEY_E);
        this->d3 -= isDown(Window::KEY_D);
    }
};

int main(int argc, char* argv[])
{
    //  auto popupData = popup::show();

    OJ_UNUSED(argc);
    OJ_UNUSED(argv);
    int width = 1600 * 0.8;
    int height = 900 * 0.8;
    bool fullScreen = false;
    bool showCursor = !fullScreen;

    /*#ifndef _DEBUG
    fullScreen = true;
    showMouse = false;
	#endif
    if (argc >= 3) {
        width = std::stoi(argv[1]);
        height = std::stoi(argv[2]);
        if (argc >= 4) {
            full = static_cast<bool>(std::stoi(argv[3]));
        }
    }
    */

    ShaderReader::setBasePath("examples/");
    ShaderReader::preLoad("shaders/edison.vs", resources::vertex::edison);
    ShaderReader::preLoad("shaders/demo.vs", resources::vertex::demo);
    ShaderReader::preLoad("shaders/post.vs", resources::vertex::post);
    ShaderReader::preLoad("shaders/fxaa.vs", resources::vertex::fxaa);
    ShaderReader::preLoad("shaders/fxaa.fs", resources::fragment::fxaa);
    ShaderReader::preLoad("shaders/post.fs", resources::fragment::post);
    ShaderReader::preLoad("shaders/lavaIntro.fs", resources::fragment::lavaIntro);
    ShaderReader::preLoad("shaders/mountain.fs", resources::fragment::mountain);
    ShaderReader::preLoad("shaders/mountainNoise.fs", resources::fragment::mountainNoise);
    ShaderReader::preLoad("shaders/mountainPost.fs", resources::fragment::mountainPost);
    ShaderReader::preLoad("shaders/lavaScene2.fs", resources::fragment::lavaScene2);
    ShaderReader::preLoad("shaders/outro.fs", resources::fragment::outro);
    ShaderReader::preLoad("shaders/mesh.vs", resources::vertex::mesh);
    ShaderReader::preLoad("shaders/mesh.fs", resources::fragment::mesh);

    // @todo move this into GLState? We can return a const reference to window.
    // and perhaps have a unified update() which does getMessages(), music sync update and
    // so on.
    Window window(width, height, "Eldur - OJ", fullScreen, showCursor);
    GLState glState;
    buildSceneGraph(glState, width, height);
    glState.initialize();

    auto mesh = Mesh::constructCube();

    Camera camera;
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
            camera.tick(key);
        }

        // glState["meshScene"]["mesh"].insertMesh(mesh, Matrix::scaling(0.2f) * Matrix::rotation(1, 1, 1, glState.relativeSceneTime().toSeconds()));
        //glState["meshScene"]["mesh"].insertMesh(mesh, Matrix::scaling(0.4f) * Matrix::translation(0.3, ojstd::sin(glState.relativeSceneTime().toSeconds()), 0.0));

        // TODO: Aspect ratio
        glState << UniformMatrix4fv("P", Matrix::perspective(45 * 3.14159265 / 180.0, 16 / 9.0, 0.001, 1000.0) * Matrix::translation(0.0, 0.0, -5.0));

        glState << Uniform1f("iTime", glState.relativeSceneTime().toSeconds());
        glState << Uniform1f("iGlobalTime", glState.relativeSceneTime().toSeconds() - 2.f);
        glState << Uniform2f("iResolution", static_cast<float>(width), static_cast<float>(height));
        glState << Uniform1f("DEBUG_D1", camera.d1);
        glState << Uniform1f("DEBUG_D2", camera.d2);
        glState << Uniform1f("DEBUG_D3", camera.d3);
        glState.update();

        timer.end();
        ojstd::sleep(33); // Are OpenGL calls async?
    }
}

extern "C" int _tmain(int argc, char** argv)
{
    return main(argc, argv);
}
