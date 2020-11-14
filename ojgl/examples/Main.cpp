#include "EmbeddedResources.h"
#include "FreeCameraController.h"
#include "demo/Demo.h"
#include "demos/Eldur.h"
#include "demos/Template.h"
#include "render/GLState.h"
#include "render/Popup.h"
#include "render/Texture.h"
#include "render/Window.h"
#include "utility/Log.h"
#include "utility/Macros.h"
#include "utility/OJstd.h"
#include "utility/ShaderReader.h"
#include "winapi/gl_loader.h"

using namespace ojgl;

static Vector2i cropToAspectRatio(const Vector2i& windowSize, float aspectRatio);

enum class DemoType {
    DodensTriumf,
    Eldur,
    InnerSystemLab,
    QED,
    Template
};

ojstd::shared_ptr<Demo> getDemo(DemoType type)
{
    switch (type) {
    case DemoType::Eldur:
        return ojstd::make_shared<Eldur>();
    case DemoType::Template:
        return ojstd::make_shared<Template>();
    }

    _ASSERTE(false);
    return nullptr;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    auto popupData = popup::show();

    Vector2i windowSize(popupData.width, popupData.height);
    bool fullScreen = popupData.full;
    bool showCursor = !fullScreen;

    ShaderReader::setBasePath("examples/shaders/");

    for (const auto& [content, path] : resources::shaders)
        ShaderReader::preLoad(path, content);

    ojstd::shared_ptr<Demo> demo = getDemo(DemoType::Template);
    Window window(windowSize, demo->getTitle(), fullScreen, showCursor);
    load_gl_functions();

    const Vector2i sceneSize = cropToAspectRatio(windowSize, 16.0 / 9.0f);
    GLState glState(window, sceneSize, resources::songs::song, demo);

    FreeCameraController cameraController;

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

        //glState["meshScene"]["mesh"].insertMesh(mesh, Matrix::scaling(0.2f) * Matrix::rotation(1, 1, 1, glState.relativeSceneTime().toSeconds()));
        // Right multiply P with cameraMatrix.inverse() and set the correct fov to use the camera controller in mesh scenes.
        glState << UniformMatrix4fv("P", Matrix::perspective(45.0f * 3.14159265f / 180.0f, static_cast<float>(sceneSize.x) / sceneSize.y, 0.001f, 1000.0f) * Matrix::translation(0.0, 0.0, -5.0));
        //  glState << Uniform1f("iTime", glState.relativeSceneTime().toSeconds());
        //glState << Uniform2f("iResolution", static_cast<float>(sceneSize.x), static_cast<float>(sceneSize.y));
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

static Vector2i cropToAspectRatio(const Vector2i& windowSize, float aspectRatio)
{
    const float windowAspectRatio = static_cast<float>(windowSize.x) / windowSize.y;

    if (aspectRatio > windowAspectRatio) {
        return Vector2i(windowSize.x, ojstd::ftoi(windowSize.x / aspectRatio));
    } else {
        return Vector2i(ojstd::ftoi(windowSize.y * aspectRatio), windowSize.y);
    }
}

extern "C" int _tmain(int argc, char** argv)
{
    return main(argc, argv);
}
