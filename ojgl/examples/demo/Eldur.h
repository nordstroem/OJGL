#pragma once
#include "Demo.h"

namespace ojgl {
class Eldur final : public Demo {
public:
    Eldur() = default;
    ojstd::vector<Scene> buildSceneGraph(const Vector2i& sceneSize) const override;
};
}
