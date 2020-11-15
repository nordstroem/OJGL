#pragma once
#include "render/Scene.h"
#include "utility/Vector.h"
#include "utility/ojstd.h"

namespace ojgl {

class Demo {
public:
    virtual ~Demo() = default;
    virtual ojstd::vector<Scene> buildSceneGraph(const Vector2i& sceneSize) const = 0;
    virtual ojstd::string getTitle() const { return ""; };
    virtual const unsigned char* getSong() const { return nullptr; };
    virtual float getAspectRatio() const { return 16.f / 9.f; };
};

}
