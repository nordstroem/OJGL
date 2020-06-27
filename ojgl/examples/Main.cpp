
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

Vector2i calculateDimensions(float demoAspectRatio, int windowWidth, int windowHeight);
void buildSceneGraph(GLState& glState, int width, int height);

int main(int argc, char* argv[])
{
    auto popupData = popup::show();

    OJ_UNUSED(argc);
    OJ_UNUSED(argv);
    int width = popupData.width;
    int height = popupData.height;
    bool fullScreen = popupData.full;
    bool showCursor = !fullScreen;

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
    FreeCameraController cameraController;

    auto [sceneWidth, sceneHeight] = calculateDimensions(16.0f / 9.0f, width, height);
    Vector2i viewportOffset((width - sceneWidth) / 2, (height - sceneHeight) / 2);

    buildSceneGraph(glState, sceneWidth, sceneHeight);
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
        glState << UniformMatrix4fv("P", Matrix::perspective(45.0f * 3.14159265f / 180.0f, static_cast<float>(sceneWidth) / sceneHeight, 0.001f, 1000.0f) * Matrix::translation(0.0, 0.0, -5.0));
        glState << Uniform1f("iTime", glState.relativeSceneTime().toSeconds());
        glState << Uniform1f("iGlobalTime", glState.relativeSceneTime().toSeconds() - 2.f);
        glState << Uniform2f("iResolution", static_cast<float>(sceneWidth), static_cast<float>(sceneHeight));
        glState << UniformMatrix4fv("iCameraMatrix", cameraMatrix);
        glState.update(viewportOffset);

        timer.end();

#ifdef _DEBUG
        ojstd::string debugTitle("Frame time: ");
        debugTitle.append(ojstd::to_string(timer.elapsed().toMilliseconds<long>()));
        debugTitle.append(" ms");
        window.setTitle(debugTitle);
#endif
    }
}

Vector2i calculateDimensions(float demoAspectRatio, int windowWidth, int windowHeight)
{
    float windowAspectRatio = static_cast<float>(windowWidth) / windowHeight;

    if (demoAspectRatio > windowAspectRatio) {
        return Vector2i(windowWidth, ojstd::ftoi(windowWidth / demoAspectRatio));
    } else {
        return Vector2i(ojstd::ftoi(windowHeight * demoAspectRatio), windowHeight);
    }
}

void buildSceneGraph(GLState& glState, int width, int height)
{
    glState.clearScenes();

    {
        auto geometry = Buffer::construct(width, height, "shaders/edison.vs", "shaders/cachedGeometry.fs");
        geometry->setFormat(BufferFormat::Quad).setRenderOnce(true).setNumOutTextures(2);

        auto lightning = Buffer::construct(width, height, "shaders/edison.vs", "shaders/lightning.fs");
        lightning->setInputs(geometry);

        glState.addScene("cachedGeometryScene", lightning, Duration::seconds(20));
    }
    {
        //auto edison = Buffer::construct(BufferFormat::Quad, x, y, "intro", "shaders/edison.vs", "shaders/lavaIntro.fs");
        //auto fxaa = Buffer::construct(BufferFormat::Quad, x, y, "fxaa", "shaders/fxaa.vs", "shaders/fxaa.fs", edison);
        //auto post = Buffer::construct(BufferFormat::Quad, x, y, "post", "shaders/post.vs", "shaders/post.fs", fxaa);

        auto mesh = Buffer::construct(width, height, "shaders/mesh.vs", "shaders/mesh.fs");
        mesh->setFormat(BufferFormat::Meshes);
        mesh->setName("mesh");
        mesh->setDepthTest(true);

        glState.addScene("meshScene", mesh, Duration::seconds(20));
    }
    {
        auto noise = Buffer::construct(width, height, "shaders/demo.vs", "shaders/mountainNoise.fs");
        auto mountain = Buffer::construct(width, height, "shaders/demo.vs", "shaders/mountain.fs");
        mountain->setInputs(noise);

        auto fxaa = Buffer::construct(width, height, "shaders/fxaa.vs", "shaders/fxaa.fs");
        fxaa->setInputs(mountain);

        auto post = Buffer::construct(width, height, "shaders/demo.vs", "shaders/mountainPost.fs");
        post->setInputs(fxaa);

        glState.addScene("introScene", post, Duration::seconds(77));
    }

    {
        auto edison = Buffer::construct(width, height, "shaders/edison.vs", "shaders/lavaScene2.fs");
        auto fxaa = Buffer::construct(width, height, "shaders/fxaa.vs", "shaders/fxaa.fs");
        fxaa->setInputs(edison);

        auto post = Buffer::construct(width, height, "shaders/post.vs", "shaders/post.fs");
        post->setInputs(fxaa);

        glState.addScene("introScene", post, Duration::seconds(40));
    }
    {
        auto edison = Buffer::construct(width, height, "shaders/edison.vs", "shaders/outro.fs");
        auto fxaa = Buffer::construct(width, height, "shaders/fxaa.vs", "shaders/fxaa.fs");
        fxaa->setInputs(edison);

        auto post = Buffer::construct(width, height, "shaders/post.vs", "shaders/post.fs");
        post->setInputs(fxaa);

        glState.addScene("introScene", post, Duration::seconds(40));
    }
}

extern "C" int _tmain(int argc, char** argv)
{
    return main(argc, argv);
}
