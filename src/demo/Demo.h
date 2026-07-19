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
    virtual float getAspectRatio() const { return 16.f / 9.f; }
    virtual void update(const Duration& relativeSceneTime, const Duration& elapsedTime, const ojstd::string& currentScene) const
    {
        OJ_UNUSED(relativeSceneTime);
        OJ_UNUSED(elapsedTime);
        OJ_UNUSED(currentScene);
    }
};

// Constructs the production selected at build time (OJGL_DEMO). Each production defines this
// in its own .cpp; exactly one production is compiled per build, so exactly one definition is
// linked. This is how Main.cpp instantiates the demo without depending on its concrete class.
ojstd::shared_ptr<Demo> createSelectedDemo();

}
