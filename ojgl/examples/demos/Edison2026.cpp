#include "Edison2026.h"
#include "FreeCameraController.h"
#include "music/Music.h"

using namespace ojgl;

ojstd::vector<Scene> Edison2026::buildSceneGraph(const Vector2i& sceneSize) const
{
    ojstd::vector<Scene> scenes;
    {
        auto cube = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2026/cube.fs");
        cube->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<UniformMatrix4fv>("iCameraMatrix", FreeCameraController::instance().getCameraMatrix()));
            auto music = Music::instance();
            vector.push_back(ojstd::make_shared<Uniform1f>("C_0_S", music->syncChannels()[10].getTimeSinceAnyNote().toSeconds()));
            vector.push_back(ojstd::make_shared<Uniform1f>("C_0_T", static_cast<float>(music->syncChannels()[10].getTotalHits())));
            return vector;
        });

        scenes.emplace_back(cube, Duration::seconds(9999), "cube");
    }

    return scenes;
}

ojstd::string Edison2026::getTitle() const
{
    return "Edison 2026";
}
