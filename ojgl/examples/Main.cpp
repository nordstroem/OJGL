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

using namespace ojgl;

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

    ojstd::shared_ptr<Demo> demo = getDemo(DemoType::Eldur);
    Window window(windowSize, demo->getTitle(), fullScreen, showCursor);

    const Vector2i sceneSize = windowSize.cropToAspectRatio(16.0f / 9.0f);
    GLState glState(window, sceneSize, resources::songs::song, demo);

    while (!glState.end() && !window.isClosePressed()) {
        Timer timer;
        timer.start();
#ifdef _DEBUG
        FreeCameraController::instance().update(window);
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
                const FreeCameraController& c = FreeCameraController::instance();
                LOG_INFO("Camera: (" << c.position.x << ", " << c.position.y << ", " << c.position.z << ")"
                                     << ", [" << c.heading << ", " << c.elevation << "]");
                break;
#endif
            }
        }

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

extern "C" int _tmain(int argc, char** argv)
{
    return main(argc, argv);
}
