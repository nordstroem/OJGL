
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
        glState.addScene("raymarch scene", buffer, Duration::seconds(1000));
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

class FreeCameraController {
public:
    void update(const Window& window)
    {
        Vec2i cursorPosition = { window.getCursorPosition().first, window.getCursorPosition().second };
        auto downKeys = window.getDownKeys();

        if (window.isLeftMouseButtonDown()) {
            int dx = ojstd::sign(cursorPosition.x - this->_previousCursorPosition.x);
            int dy = ojstd::sign(cursorPosition.y - this->_previousCursorPosition.y);
            this->_heading -= this->_rotationSpeed * dx;
            this->_elevation -= this->_rotationSpeed * dy;
        }

        if (downKeys.contains(Window::KEY_W)) {
            this->_position.z -= this->_translationSpeed * ojstd::cos(this->_heading);
            this->_position.x -= this->_translationSpeed * ojstd::sin(this->_heading);
        }
        if (downKeys.contains(Window::KEY_S)) {
            this->_position.z += this->_translationSpeed * ojstd::cos(this->_heading);
            this->_position.x += this->_translationSpeed * ojstd::sin(this->_heading);
        }
        if (downKeys.contains(Window::KEY_D)) {
            this->_position.z += this->_translationSpeed * ojstd::cos(this->_heading + ojstd::pi / 2.f);
            this->_position.x += this->_translationSpeed * ojstd::sin(this->_heading + ojstd::pi / 2.f);
        }
        if (downKeys.contains(Window::KEY_A)) {
            this->_position.z -= this->_translationSpeed * ojstd::cos(this->_heading + ojstd::pi / 2.f);
            this->_position.x -= this->_translationSpeed * ojstd::sin(this->_heading + ojstd::pi / 2.f);
        }
        if (downKeys.contains(Window::KEY_Z))
            this->_position.y += this->_translationSpeed;
        if (downKeys.contains(Window::KEY_X))
            this->_position.y -= this->_translationSpeed;

        this->_previousCursorPosition = cursorPosition;
    }

    Matrix getCameraMatrix() const
    {
        float sh = ojstd::sin(this->_heading);
        float se = ojstd::sin(this->_elevation);
        float ch = ojstd::cos(this->_heading);
        float ce = ojstd::cos(this->_elevation);
        float px = this->_position.x;
        float py = this->_position.y;
        float pz = this->_position.z;
        float rotationMatrix[16] = { ch, 0, -sh, 0, se * sh, ce, ch * se, 0, sh * ce, -se, ch * ce, 0, px, py, pz, 1 };
        return Matrix(rotationMatrix);
    }

private:
    Vec3f _position { 0, 0, 2 };
    float _heading = 0.f;
    float _elevation = 0.f;
    Vec2i _previousCursorPosition;

    static constexpr float _translationSpeed = 0.025f;
    static constexpr float _rotationSpeed = 0.02f;
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
    FreeCameraController cameraController;

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
        glState << UniformMatrix4fv("iCameraMatrix", cameraController.getCameraMatrix());

        glState.update();

        timer.end();
        ojstd::sleep(10); // Are OpenGL calls async?
    }
}

extern "C" int _tmain(int argc, char** argv)
{
    return main(argc, argv);
}
