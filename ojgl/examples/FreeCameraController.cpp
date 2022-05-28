#include "FreeCameraController.h"
#include "utility/Log.h"

using namespace ojgl;

static FreeCameraController controller;

FreeCameraController::FreeCameraController()
    : _previousUpdateTime(Timepoint::now())
{
}

FreeCameraController& FreeCameraController::instance()
{
    return controller;
}

void FreeCameraController::update(const Window& window)
{
    Vector2i cursorPosition = { window.getCursorPosition().x, window.getCursorPosition().y };
    auto downKeys = window.getDownKeys();

    float dt = (Timepoint::now() - this->_previousUpdateTime).toMilliseconds();
    this->_previousUpdateTime = Timepoint::now();

    if (window.isLeftMouseButtonDown()) {
        int dx = ojstd::sign(cursorPosition.x - this->_previousCursorPosition.x);
        int dy = ojstd::sign(cursorPosition.y - this->_previousCursorPosition.y);
        this->heading -= this->_rotationSpeed * dt * static_cast<float>(dx);
        this->elevation -= this->_rotationSpeed * dt * static_cast<float>(dy);
    }

    if (downKeys.contains(Window::KEY_W)) {
        this->position.z -= this->_translationSpeed * dt * ojstd::cos(this->heading);
        this->position.x -= this->_translationSpeed * dt * ojstd::sin(this->heading);
    }
    if (downKeys.contains(Window::KEY_S)) {
        this->position.z += this->_translationSpeed * dt * ojstd::cos(this->heading);
        this->position.x += this->_translationSpeed * dt * ojstd::sin(this->heading);
    }
    if (downKeys.contains(Window::KEY_D)) {
        this->position.z += this->_translationSpeed * dt * ojstd::cos(this->heading + ojstd::pi / 2.f);
        this->position.x += this->_translationSpeed * dt * ojstd::sin(this->heading + ojstd::pi / 2.f);
    }
    if (downKeys.contains(Window::KEY_A)) {
        this->position.z -= this->_translationSpeed * dt * ojstd::cos(this->heading + ojstd::pi / 2.f);
        this->position.x -= this->_translationSpeed * dt * ojstd::sin(this->heading + ojstd::pi / 2.f);
    }
    if (downKeys.contains(Window::KEY_Z))
        this->position.y += this->_translationSpeed * dt;
    if (downKeys.contains(Window::KEY_X))
        this->position.y -= this->_translationSpeed * dt;

    if (downKeys.contains(Window::KEY_K)) {
        LOG_INFO("Position: {" << this->position.x << ", " << this->position.y << ", " << this->position.z << "}, Heading: " << this->heading << ", Elevation: " << this->elevation);
    }

    this->_previousCursorPosition = cursorPosition;
}

Matrix FreeCameraController::getCameraMatrix() const
{
    float sh = ojstd::sin(this->heading);
    float se = ojstd::sin(this->elevation);
    float ch = ojstd::cos(this->heading);
    float ce = ojstd::cos(this->elevation);
    float px = this->position.x;
    float py = this->position.y;
    float pz = this->position.z;
    float rotationMatrix[16] = { ch, 0, -sh, 0, se * sh, ce, ch * se, 0, sh * ce, -se, ch * ce, 0, px, py, pz, 1 };
    return Matrix(rotationMatrix);
}

void FreeCameraController::set(const Vector3f& newPosition, float newHeading, float newElevation)
{
    this->position = newPosition;
    this->heading = newHeading;
    this->elevation = newElevation;
}
