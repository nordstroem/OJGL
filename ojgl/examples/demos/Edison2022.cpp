#include "Edison2022.h"
#include "FreeCameraController.h"

using namespace ojgl;

Edison2022::Edison2022()
{
    FreeCameraController::instance().set({ 9.7f, 19.5f, 28.6f }, 0.32f, -0.5f);
}

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

        auto fxaa = Buffer::construct(sceneSize.x, sceneSize.y, "common/fxaa.vs", "common/fxaa.fs");
        fxaa->setInputs(raymarch);

        auto chrom = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2022/chrom_ab.fs");
        chrom->setInputs(fxaa);
        scenes.emplace_back(fxaa, Duration::seconds(9999), "scene0");
    }

    return scenes;
}

ojstd::string Edison2022::getTitle() const
{
    return "Edison 2022";
}
