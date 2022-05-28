#pragma once

#include "render/Window.h"
#include "utility/Matrix.h"
#include "utility/OJstd.h"
#include "utility/Timepoint.h"
#include "utility/Vector.h"

namespace ojgl {

class FreeCameraController {
public:
    Vector3f position { 0, 0, 0 };
    float heading = 0.f;
    float elevation = 0.f;

public:
    static FreeCameraController& instance();
    FreeCameraController();

    void update(const Window& window);
    Matrix getCameraMatrix() const;
    void set(const Vector3f& newPosition, float newHeading, float newElevation);

private:
    static constexpr float _translationSpeed = 0.01f;
    static constexpr float _rotationSpeed = 0.002f;

    Vector2i _previousCursorPosition;
    Timepoint _previousUpdateTime;
};
}
