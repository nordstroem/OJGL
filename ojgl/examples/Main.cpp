
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
        auto smoke = Buffer::construct(x, y, "smoke", "shaders/geometry-with-physics/smoke.vs", "shaders/geometry-with-physics/smoke.fs", {});

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
    //auto popupData = popup::show();

    OJ_UNUSED(argc);
    OJ_UNUSED(argv);
    int width = 1280;
    int height = 720;
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

    auto mesh = Mesh::constructQuad();
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

        float baseTime = glState.relativeSceneTime().toSeconds();
        auto [fraction, base] = ojstd::modf(baseTime);
        float time = base + ojstd::pow(fraction, 2);

        Matrix cameraMatrix = cameraController.getCameraMatrix();
        Matrix cameraMatrixInverse = cameraMatrix.inverse();

        auto billboardMatrix = [&cameraMatrixInverse](const Vector3f& position, const float scaling) {
            const float* md = cameraMatrixInverse.data();
            float bd[16] = { md[0] * scaling, md[4], md[8], 0, md[1], md[5] * scaling, md[9], 0, md[2], md[6], md[10] * scaling, 0, position.x, position.y, position.z, 1 };
            return Matrix(bd);
        };

        //for (int i = 0; i < 10; i++) {
            //    glState["meshScene"]["mesh"].insertMesh(mesh, billboardMatrix({ 1.0f + i / 2.f, i / 5.f, 0.f }, 1.0));
        //}
        
        glState["meshScene"]["mesh"].insertMesh(mesh, billboardMatrix({ 3.0f, 1.f, 0.f }, 1.0));


        /*for (int i = 0; i < 200; i++) {
            float xPos = (i / 200.f - 0.5f) * 4;
            float yPos = ojstd::sin(xPos * 5.f + 0.1 * time) * ojstd::cos(xPos * 2.5f + 1.0 * time);
            float zPos = ojstd::sin(xPos * 4.f + 0.1 * time) * ojstd::cos(yPos * 3.5f + 1.0 * time);
            glState["meshScene"]["mesh"].insertMesh(mesh, Matrix::translation(xPos, yPos, zPos) * Matrix::rotation(1, 1, 1, time + i) * Matrix::scaling(0.06f * ojstd::sin(xPos + time)));
        }*/
        /*
        for (int i = 0; i < 200; i++) {
            baseTime += 5.;
            float xPos = (i / 200.f - 0.5f) * 4;
            float yPos = 2 * ojstd::sin(xPos * 5.f + 0.1 * baseTime) * ojstd::cos(xPos * 2.5f + 1.0 * baseTime);
            float zPos = 0.1 * ojstd::sin(xPos * 4.f + 0.1 * baseTime) * ojstd::cos(yPos * 3.5f + 1.0 * baseTime);
            glState["meshScene"]["mesh"].insertMesh(mesh, Matrix::translation(xPos, yPos, zPos) * Matrix::rotation(1, 1, 1, baseTime + i) * Matrix::scaling(0.01f));
        }
		*/
        //glState["meshScene"]["mesh"].insertMesh(mesh, Matrix::translation(1, 0, 0) * Matrix::scaling(0.2f) * Matrix::rotation(1, 1, 1, glState.relativeSceneTime().toSeconds()));

        //glState["meshScene"]["mesh"].insertMesh(mesh, Matrix::scaling(0.4f) * Matrix::translation(0.3, ojstd::sin(glState.relativeSceneTime().toSeconds()), 0.0));

        float fov = 0.927295218f;
        float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
        glState << UniformMatrix4fv("P", Matrix::perspective(fov, aspectRatio, 0.001f, 100.0f) * cameraMatrixInverse);

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
