#include "QED.h"

using namespace ojgl;

ojstd::vector<Scene> QED::buildSceneGraph(const Vector2i& sceneSize) const
{
    ojstd::vector<Scene> scenes;
    {
        auto raymarch = Buffer::construct(sceneSize.x / 4, sceneSize.y / 4, "edison.vs", "common/raymarch_template.fs");
        scenes.emplace_back(raymarch, Duration::seconds(9999), "raymarchScene");
    }

    return scenes;
}

ojstd::string QED::getTitle() const
{
    return "OJ - Q.E.D.";
}
