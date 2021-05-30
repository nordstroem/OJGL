#include "Edison2021.h"
#include "FreeCameraController.h"
#include "music/Music.h"

using namespace ojgl;

ojstd::string Edison2021::getTitle() const
{
    return "OJ - Edison 2021";
}

const unsigned char* Edison2021::getSong() const
{
    return nullptr;
}

ojstd::vector<Scene> Edison2021::buildSceneGraph(const Vector2i& sceneSize) const
{
    ojstd::vector<Scene> scenes;
    {
        auto raymarch = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2021/main.fs");
        raymarch->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<UniformMatrix4fv>("iCameraMatrix", FreeCameraController::instance().getCameraMatrix()));
            return vector;
        });

        auto fxaa = Buffer::construct(sceneSize.x, sceneSize.y, "common/fxaa.vs", "common/fxaa.fs");
        fxaa->setInputs(raymarch);

        scenes.emplace_back(fxaa, Duration::seconds(9999), "raymarchScene");
    }

    return scenes;
}