#pragma once
#include "demo/Demo.h"

namespace ojgl {
class Eldur final : public Demo {
public:
    ojstd::vector<Scene> buildSceneGraph(const Vector2i& sceneSize) const override;
    ojstd::string getTitle() const override;
    unsigned char* getSong() const override;
};
}
