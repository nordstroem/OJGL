
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

void buildSceneGraph(GLState& glState, const Vector2i& sceneSize);

int main(int argc, char* argv[])
{
    auto popupData = popup::show();

    OJ_UNUSED(argc);
    OJ_UNUSED(argv);
    int width = popupData.width;
    int height = popupData.height;
    bool fullScreen = popupData.full;
    bool showCursor = !fullScreen;

    ShaderReader::setBasePath("examples/shaders/");

    for (const auto& [content, path] : resources::shaders)
        ShaderReader::preLoad(path, content);

    // @todo move this into GLState? We can return a const reference to window.
    // and perhaps have a unified update() which does getMessages(), music sync update and
    // so on.
    Window window(width, height, "Eldur - OJ", fullScreen, showCursor);
    FreeCameraController cameraController;

    GLState glState(window, 16.0f / 9.0f, resources::songs::song);

    Vector2i sceneSize = glState.sceneSize();
    buildSceneGraph(glState, sceneSize);
    glState.initialize();

    auto mesh = Mesh::constructCube();

    while (!glState.end() && !window.isClosePressed()) {
        Timer timer;
        timer.start();
#ifdef _DEBUG
        cameraController.update(window);
#endif
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

            case Window::KEY_C:
                LOG_INFO("Camera: (" << cameraController.position.x << ", " << cameraController.position.y << ", " << cameraController.position.z << ")"
                                     << ", [" << cameraController.heading << ", " << cameraController.elevation << "]");
                break;
#endif
            }
        }

        Matrix cameraMatrix = cameraController.getCameraMatrix();

        glState["meshScene"]["mesh"].insertMesh(mesh, Matrix::scaling(0.2f) * Matrix::rotation(1, 1, 1, glState.relativeSceneTime().toSeconds()));
        // Right multiply P with cameraMatrix.inverse() and set the correct fov to use the camera controller in mesh scenes.
        glState << UniformMatrix4fv("P", Matrix::perspective(45.0f * 3.14159265f / 180.0f, static_cast<float>(sceneSize.x) / sceneSize.y, 0.001f, 1000.0f) * Matrix::translation(0.0, 0.0, -5.0));
        glState << Uniform1f("iTime", glState.relativeSceneTime().toSeconds());
        glState << Uniform1f("iGlobalTime", glState.relativeSceneTime().toSeconds() - 2.f);
        glState << Uniform2f("iResolution", static_cast<float>(sceneSize.x), static_cast<float>(sceneSize.y));
        glState << UniformMatrix4fv("iCameraMatrix", cameraMatrix);
        glState.update();

        timer.end();

#ifdef _DEBUG
        ojstd::string debugTitle("Frame time: ");
        debugTitle.append(ojstd::to_string(timer.elapsed().toMilliseconds<long>()));
        debugTitle.append(" ms");
        window.setTitle(debugTitle);
#endif
    }
}

void buildSceneGraph(GLState& glState, const Vector2i& sceneSize)
{
    glState.clearScenes();

    {
        auto raymarch = Buffer::construct(sceneSize.x / 4, sceneSize.y / 4, "edison.vs", "common/raymarch_template.fs");
        glState.addScene("raymarchScene", raymarch, Duration::seconds(9999));
    }

    {
        auto geometry = Buffer::construct(sceneSize.x, sceneSize.y, "edison.vs", "cachedGeometry.fs");
        geometry->setFormat(BufferFormat::Quad).setRenderOnce(true).setNumOutTextures(2);

        auto lightning = Buffer::construct(sceneSize.x, sceneSize.y, "edison.vs", "lightning.fs");
        lightning->setInputs(geometry);

        glState.addScene("cachedGeometryScene", lightning, Duration::seconds(20));
    }
    {
        //auto edison = Buffer::construct(BufferFormat::Quad, x, y, "intro", "shaders/edison.vs", "shaders/lavaIntro.fs");
        //auto fxaa = Buffer::construct(BufferFormat::Quad, x, y, "fxaa", "shaders/fxaa.vs", "shaders/fxaa.fs", edison);
        //auto post = Buffer::construct(BufferFormat::Quad, x, y, "post", "shaders/post.vs", "shaders/post.fs", fxaa);

        auto mesh = Buffer::construct(sceneSize.x, sceneSize.y, "mesh.vs", "mesh.fs");
        mesh->setFormat(BufferFormat::Meshes);
        mesh->setName("mesh");
        mesh->setDepthTest(true);

        glState.addScene("meshScene", mesh, Duration::seconds(20));
    }
    {
        auto noise = Buffer::construct(sceneSize.x, sceneSize.y, "demo.vs", "mountainNoise.fs");
        auto mountain = Buffer::construct(sceneSize.x, sceneSize.y, "demo.vs", "mountain.fs");
        mountain->setInputs(noise);

        auto fxaa = Buffer::construct(sceneSize.x, sceneSize.y, "fxaa.vs", "fxaa.fs");
        fxaa->setInputs(mountain);

        auto post = Buffer::construct(sceneSize.x, sceneSize.y, "demo.vs", "mountainPost.fs");
        post->setInputs(fxaa);

        glState.addScene("introScene", post, Duration::seconds(77));
    }

    {
        auto edison = Buffer::construct(sceneSize.x, sceneSize.y, "edison.vs", "lavaScene2.fs");
        auto fxaa = Buffer::construct(sceneSize.x, sceneSize.y, "fxaa.vs", "fxaa.fs");
        fxaa->setInputs(edison);

        auto post = Buffer::construct(sceneSize.x, sceneSize.y, "post.vs", "post.fs");
        post->setInputs(fxaa);

        glState.addScene("introScene", post, Duration::seconds(40));
    }
    {
        auto edison = Buffer::construct(sceneSize.x, sceneSize.y, "edison.vs", "outro.fs");
        auto fxaa = Buffer::construct(sceneSize.x, sceneSize.y, "fxaa.vs", "fxaa.fs");
        fxaa->setInputs(edison);

        auto post = Buffer::construct(sceneSize.x, sceneSize.y, "post.vs", "post.fs");
        post->setInputs(fxaa);

        glState.addScene("introScene", post, Duration::seconds(40));
    }
}

extern "C" int _tmain(int argc, char** argv)
{
    return main(argc, argv);
}
