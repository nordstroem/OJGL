#pragma once

#include "render/Window.h"
#include "utility/Matrix.h"
#include "utility/OJstd.h"
#include "utility/Vector.h"

namespace ojgl {

class FreeCameraController {

public:
    void update(const Window& window)
    {
        Vector2i cursorPosition = { window.getCursorPosition().x, window.getCursorPosition().y };
        auto downKeys = window.getDownKeys();

        if (window.isLeftMouseButtonDown()) {
            int dx = ojstd::sign(cursorPosition.x - this->_previousCursorPosition.x);
            int dy = ojstd::sign(cursorPosition.y - this->_previousCursorPosition.y);
            this->heading -= this->_rotationSpeed * dx;
            this->elevation -= this->_rotationSpeed * dy;
        }

        if (downKeys.contains(Window::KEY_W)) {
            this->position.z -= this->_translationSpeed * ojstd::cos(this->heading);
            this->position.x -= this->_translationSpeed * ojstd::sin(this->heading);
        }
        if (downKeys.contains(Window::KEY_S)) {
            this->position.z += this->_translationSpeed * ojstd::cos(this->heading);
            this->position.x += this->_translationSpeed * ojstd::sin(this->heading);
        }
        if (downKeys.contains(Window::KEY_D)) {
            this->position.z += this->_translationSpeed * ojstd::cos(this->heading + ojstd::pi / 2.f);
            this->position.x += this->_translationSpeed * ojstd::sin(this->heading + ojstd::pi / 2.f);
        }
        if (downKeys.contains(Window::KEY_A)) {
            this->position.z -= this->_translationSpeed * ojstd::cos(this->heading + ojstd::pi / 2.f);
            this->position.x -= this->_translationSpeed * ojstd::sin(this->heading + ojstd::pi / 2.f);
        }
        if (downKeys.contains(Window::KEY_Z))
            this->position.y += this->_translationSpeed;
        if (downKeys.contains(Window::KEY_X))
            this->position.y -= this->_translationSpeed;

        this->_previousCursorPosition = cursorPosition;
    }

    Matrix getCameraMatrix() const
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

    void set(const Vector3f& position, float heading, float elevation)
    {
        this->position = position;
        this->heading = heading;
        this->elevation = elevation;
    }

    Vector3f position { 0, 0, 2 };
    float heading = 0.f;
    float elevation = 0.f;

private:
    Vector2i _previousCursorPosition;

    static constexpr float _translationSpeed = 0.025f;
    static constexpr float _rotationSpeed = 0.02f;
};
}
