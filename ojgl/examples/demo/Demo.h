#pragma once
#include "render/Scene.h"
#include "utility/Vector.h"
#include "utility/ojstd.h"

namespace ojgl {

enum class DemoType {
    Eldur
};

class Demo {
public:
    virtual ~Demo() = default;
    virtual ojstd::vector<Scene> buildSceneGraph(const Vector2i& sceneSize) const = 0;
};

ojstd::shared_ptr<Demo> getDemo(DemoType type);

}