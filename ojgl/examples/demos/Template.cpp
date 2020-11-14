#include "Template.h"
#include "FreeCameraController.h"

using namespace ojgl;

ojstd::vector<Scene> Template::buildSceneGraph(const Vector2i& sceneSize) const
{
    ojstd::vector<Scene> scenes;
    {
        auto raymarch = Buffer::construct(sceneSize.x / 4, sceneSize.y / 4, "edison.vs", "common/raymarch_template.fs");
        raymarch->setUniformCallback([](float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<UniformMatrix4fv>("iCameraMatrix", FreeCameraController::instance().getCameraMatrix()));
            return vector;
        });

        scenes.emplace_back(raymarch, Duration::seconds(9999), "raymarchScene");
    }

    return scenes;
}

ojstd::string Template::getTitle() const
{
    return "Template Demo";
}
