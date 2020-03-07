
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

struct Vec3f {
    float x = 0.f;
    float y = 0.f;
    float z = 0.f;
};

struct Vec2i {
    int x = 0;
    int y = 0;
};

class CameraController {
public:
    void update(const Window& window)
    {
        Vec2i cursorPosition = { window.getCursorPosition().first, window.getCursorPosition().second };
        auto downKeys = window.getDownKeys();

        if (window.isLeftMouseButtonDown()) {
            float dr = 0.01f;
            int dx = ojstd::sign(cursorPosition.x - this->_previousCursorPosition.x);
            int dy = ojstd::sign(cursorPosition.y - this->_previousCursorPosition.y);
            this->_heading += dr * dx;
            this->_elevation += dr * dy;
        }

        if (downKeys.contains(Window::KEY_W))
            this->_position.z += 0.01f;
        if (downKeys.contains(Window::KEY_W))
            this->_position.z -= 0.1f;
        if (downKeys.contains(Window::KEY_D))
            this->_position.x += 0.1f;
        if (downKeys.contains(Window::KEY_A))
            this->_position.x -= 0.1f;

        this->_previousCursorPosition = cursorPosition;
    }

    Vec3f getPosition() const
    {
        return this->_position;
    }

    const float* getDirectionMatrix() const
    {
        float sh = ojstd::sin(this->_heading);
        float se = ojstd::sin(this->_elevation);
        float ch = ojstd::cos(this->_heading);
        float ce = ojstd::cos(this->_elevation);
        float rotationMatrix[9] = { ch, se * sh, sh * ce, 0, ce, -se, -sh, ch * se, ch * ce };
        for (int i = 0; i < 9; i++)
            this->_directionMatrix[i] = rotationMatrix[i];
        return this->_directionMatrix;
    }

private:
    Vec3f _position;
    float _heading = 0.f;
    float _elevation = 0.f;
    Vec2i _previousCursorPosition;
    mutable float _directionMatrix[9];
};

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
    CameraController cameraController;

    while (!glState.end() && !window.isClosePressed()) {
        Timer timer;
        timer.start();
        window.getMessages();
        cameraController.update(window);

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
        glState << UniformMatrix3fv("iCameraMatrix", cameraController.getDirectionMatrix());

        glState.update();

        timer.end();
        ojstd::sleep(10); // Are OpenGL calls async?
    }
}

extern "C" int _tmain(int argc, char** argv)
{
    return main(argc, argv);
}
