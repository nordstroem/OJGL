
#include "EmbeddedResources.h"
#include "FreeCameraController.h"
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

    /*{
        auto geometry = Buffer::construct(x, y, "geometry", "shaders/edison.vs", "shaders/cachedGeometry.fs", {}, ojgl::BufferFormat::Quad, true, 2);
        auto lightning = Buffer::construct(x, y, "lightning", "shaders/edison.vs", "shaders/lightning.fs", { geometry });
        glState.addScene("cachedGeometryScene", lightning, Duration::seconds(100));
    }*/
    {
        //auto edison = Buffer::construct(BufferFormat::Quad, x, y, "intro", "shaders/edison.vs", "shaders/lavaIntro.fs");
        //auto fxaa = Buffer::construct(BufferFormat::Quad, x, y, "fxaa", "shaders/fxaa.vs", "shaders/fxaa.fs", edison);
        //auto post = Buffer::construct(BufferFormat::Quad, x, y, "post", "shaders/post.vs", "shaders/post.fs", fxaa);

        auto mesh = Buffer::construct(x, y, "mesh", "shaders/geometry-with-physics/mesh.vs", "shaders/geometry-with-physics/mesh.fs", {}, BufferFormat::Meshes, false, 2);
        auto rayMarch = Buffer::construct(x, y, "rayMarch", "shaders/geometry-with-physics/rayMarch.vs", "shaders/geometry-with-physics/rayMarch.fs", { mesh });

        glState.addScene("meshScene", rayMarch, Duration::seconds(9999));
    }
    {
        auto noise = Buffer::construct(x, y, "intro", "shaders/demo.vs", "shaders/mountainNoise.fs");
        auto mountain = Buffer::construct(x, y, "fxaa", "shaders/demo.vs", "shaders/mountain.fs", { noise });
        auto fxaa = Buffer::construct(x, y, "fxaa", "shaders/fxaa.vs", "shaders/fxaa.fs", { mountain });
        auto post = Buffer::construct(x, y, "post", "shaders/demo.vs", "shaders/mountainPost.fs", { fxaa });
        glState.addScene("introScene", post, Duration::seconds(77));
    }

    {
        auto edison = Buffer::construct(x, y, "intro", "shaders/edison.vs", "shaders/lavaScene2.fs");
        auto fxaa = Buffer::construct(x, y, "fxaa", "shaders/fxaa.vs", "shaders/fxaa.fs", { edison });
        auto post = Buffer::construct(x, y, "post", "shaders/post.vs", "shaders/post.fs", { fxaa });
        glState.addScene("introScene", post, Duration::seconds(40));
    }
    {
        auto edison = Buffer::construct(x, y, "intro", "shaders/edison.vs", "shaders/outro.fs");
        auto fxaa = Buffer::construct(x, y, "fxaa", "shaders/fxaa.vs", "shaders/fxaa.fs", { edison });
        auto post = Buffer::construct(x, y, "post", "shaders/post.vs", "shaders/post.fs", { fxaa });
        glState.addScene("introScene", post, Duration::seconds(40));
    }
}

int main(int argc, char* argv[])
{
    auto popupData = popup::show();

    OJ_UNUSED(argc);
    OJ_UNUSED(argv);
    int width = popupData.width;
    int height = popupData.height;
    bool fullScreen = popupData.full;
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
    ShaderReader::preLoad("shaders/cachedGeometry.fs", resources::fragment::cachedGeometry);
    ShaderReader::preLoad("shaders/lightning.fs", resources::fragment::lightning);

    // @todo move this into GLState? We can return a const reference to window.
    // and perhaps have a unified update() which does getMessages(), music sync update and
    // so on.
    Window window(width, height, "Eldur - OJ", fullScreen, showCursor);
    GLState glState(resources::songs::song);
    buildSceneGraph(glState, width, height);
    glState.initialize();
    FreeCameraController cameraController;

    auto mesh = Mesh::constructCube();
    while (!glState.end() && !window.isClosePressed()) {
        Timer timer;
        timer.start();
        cameraController.update(window);
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

        float time = glState.relativeSceneTime().toSeconds();

        for (int i = 0; i < 100; i++) {
            float xPos = (i / 100.f - 0.5f) * 4;
            float yPos = ojstd::sin(xPos * 5.f + 0.1 * time) * ojstd::cos(xPos * 2.5f + 1.4 * time);
            glState["meshScene"]["mesh"].insertMesh(mesh, Matrix::translation(xPos, yPos, 0) * Matrix::rotation(1, 1, 1, time + i) * Matrix::scaling(0.08f * ojstd::sin(xPos + time)));
        }

        //glState["meshScene"]["mesh"].insertMesh(mesh, Matrix::translation(1, 0, 0) * Matrix::scaling(0.2f) * Matrix::rotation(1, 1, 1, glState.relativeSceneTime().toSeconds()));

        //glState["meshScene"]["mesh"].insertMesh(mesh, Matrix::scaling(0.4f) * Matrix::translation(0.3, ojstd::sin(glState.relativeSceneTime().toSeconds()), 0.0));

        // TODO: Aspect ratio
        Matrix cameraMatrix = cameraController.getCameraMatrix();
        float fov = 0.927295218f;
        float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
        glState << UniformMatrix4fv("P", Matrix::perspective(fov, aspectRatio, 0.001f, 100.0f) * cameraMatrix.inverse());

        glState << Uniform1f("iTime", glState.relativeSceneTime().toSeconds());
        glState << Uniform1f("iGlobalTime", glState.relativeSceneTime().toSeconds() - 2.f);
        glState << Uniform2f("iResolution", static_cast<float>(width), static_cast<float>(height));
        glState << UniformMatrix4fv("iCameraMatrix", cameraMatrix);

        glState.update();

        timer.end();
        ojstd::sleep(10); // Are OpenGL calls async?
    }
}

extern "C" int _tmain(int argc, char** argv)
{
    return main(argc, argv);
}
