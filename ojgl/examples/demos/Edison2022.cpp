#include "Edison2022.h"
#include "FreeCameraController.h"
#include "utility/Log.h"
#include "utility/Spline3.h"

using namespace ojgl;

Edison2022::Edison2022()
{
    FreeCameraController::instance().set({ 39.0531f, 50.1299f, 20.5951f }, 1.114f, -0.846f);

    //FreeCameraController::instance().set({ 0, 0, 30 }, { 0.f, 0.f, 0.f });

    // experiment.fs
    //FreeCameraController::instance().set({ 30.17f, 23.19f, 34.3f }, 2.548f, -0.374f);
}

// clang-format off
auto ts = {0.0f, 8.0f, };
float ps_x[] = {-26.1343f, 0.0f, -0.49438125000000005f, 0.041198437500000004f, };
float ps_y[] = {5.64f, 0.0f, 4.489219171875f, -0.37410159765625f, };
float ps_z[] = {43.4476f, 0.0f, -0.44643750000000004f, 0.037203125000000004f, };
// clang-format on

ojstd::vector<Polynomial3> polysX = polynomialLoad(ps_x);
ojstd::vector<Polynomial3> polysY = polynomialLoad(ps_y);
ojstd::vector<Polynomial3> polysZ = polynomialLoad(ps_z);

Spline3 spline = Spline3(ts, polysX, polysY, polysZ);

ojstd::vector<Scene> Edison2022::buildSceneGraph(const Vector2i& sceneSize) const
{
    ojstd::vector<Scene> scenes;
    {
        auto raymarch = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2022/grass.fs");
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

void Edison2022::update(const Duration& relativeSceneTime, const Duration& elapsedTime) const
{
    OJ_UNUSED(elapsedTime);
    OJ_UNUSED(relativeSceneTime);
    // auto& camera = FreeCameraController::instance();
    // Vector3f newPosition = spline(relativeSceneTime.toSeconds() * 0.5f);
   // camera.set(newPosition, { 0.f, 0.f, 0.f });
}

ojstd::string Edison2022::getTitle() const
{
    return "Edison 2022";
}
