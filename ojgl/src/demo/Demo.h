#pragma once
#include "render/Scene.h"
#include "utility/Duration.h"
#include "utility/Macros.h"
#include "utility/Vector.h"
#include "utility/ojstd.h"

namespace ojgl {

class Demo {
public:
    virtual ~Demo() = default;
    virtual ojstd::vector<Scene> buildSceneGraph(const Vector2i& sceneSize) const = 0;
    virtual ojstd::string getTitle() const { return ""; }
    virtual const unsigned char* getSong() const { return nullptr; }
    virtual float getAspectRatio() const { return 16.f / 9.f; }
    virtual void update(const Duration& relativeSceneTime, const Duration& elapsedTime) const
    {
        OJ_UNUSED(relativeSceneTime);
        OJ_UNUSED(elapsedTime);
    }
};

}
