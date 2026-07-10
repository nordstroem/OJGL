#include "FreeCameraController.h"
#include "demo/Demo.h"
#include "music/Music.h"

namespace ojgl {
class Edison2026 final : public Demo {
public:
    ojstd::vector<Scene> buildSceneGraph(const Vector2i& sceneSize) const override;
    ojstd::string getTitle() const override;
    
};
}

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
            if (music != nullptr) {
                vector.push_back(ojstd::make_shared<Uniform1f>("C_0_S", music->syncChannels()[10].getTimeSinceAnyNote().toSeconds()));
                vector.push_back(ojstd::make_shared<Uniform1f>("C_0_T", static_cast<float>(music->syncChannels()[10].getTotalHits())));
            }
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

ojstd::shared_ptr<Demo> ojgl::createSelectedDemo()
{
    return ojstd::make_shared<Edison2026>();
}
