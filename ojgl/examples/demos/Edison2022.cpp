#include "Edison2022.h"
#include "FreeCameraController.h"

using namespace ojgl;

ojstd::vector<Scene> Edison2022::buildSceneGraph(const Vector2i& sceneSize) const
{
    ojstd::vector<Scene> scenes;
    {
        auto raymarch = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2022/experiment.fs");
        raymarch->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<UniformMatrix4fv>("iCameraMatrix", FreeCameraController::instance().getCameraMatrix()));
            return vector;
        });

        scenes.emplace_back(raymarch, Duration::seconds(9999), "raymarchScene");
    }

    return scenes;
}

ojstd::string Edison2022::getTitle() const
{
    return "Edison 2022";
}
