#pragma once
#include "demo/Demo.h"
#include "render/Texture.h"

namespace ojgl {
class Edison2022 final : public Demo {
public:
    Edison2022();
    ojstd::vector<Scene> buildSceneGraph(const Vector2i& sceneSize) const override;
    ojstd::string getTitle() const override;
};
}
