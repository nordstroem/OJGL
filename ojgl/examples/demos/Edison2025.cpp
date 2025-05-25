#include "Edison2025.h"

namespace ojgl {

Edison2025::Edison2025()
{
}

ojstd::vector<ojgl::Scene> ojgl::Edison2025::buildSceneGraph(const Vector2i& sceneSize) const
{
    OJ_UNUSED(sceneSize);
    return {};
}

const unsigned char* Edison2025::getSong() const { return nullptr; }

ojstd::string Edison2025::getTitle() const
{
    return "Edison 2025";
}

void Edison2025::update(const Duration& relativeSceneTime, const Duration& elapsedTime, const ojstd::string& currentScene) const
{
    OJ_UNUSED(relativeSceneTime);
    OJ_UNUSED(elapsedTime);
    OJ_UNUSED(currentScene);
}

}