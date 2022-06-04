#include "Edison2022.h"
#include "FreeCameraController.h"
#include "utility/Log.h"
#include "utility/Spline3.h"

using namespace ojgl;

Edison2022::Edison2022()
{
    //FreeCameraController::instance().set({ 9.7f, 19.5f, 28.6f }, 0.32f, -0.5f);

    FreeCameraController::instance().set({ 0, 0, 30 }, { 0.f, 0.f, 0.f });

    // experiment.fs
    //FreeCameraController::instance().set({ 30.17f, 23.19f, 34.3f }, 2.548f, -0.374f);
}

// clang-format off
float ps[] = {0.0f, 43.417150514466115f, -44.42177648148498f, 10.369253118329068f, 8.660254037844387f, -15.506125183737897f, -11.845807061729332f, 10.369253118329068f, -8.660254037844384f, -6.202450073495167f, 20.730162358026323f, -6.598615620754854f, -2.4492935982947065e-15f, 15.506125183737907f, 7.993605777301127e-15f, -6.598615620754872f, 8.660254037844393f, -6.202450073495152f, -20.730162358026334f, 10.369253118329064f, -8.66025403784438f, -15.506125183737925f, 11.845807061729316f, 10.369253118329077f, };
auto ts = {0.0f, 1.0471975511965976f, 2.0943951023931953f, 3.141592653589793f, 4.1887902047863905f, 5.235987755982988f, };
// clang-format on

ojstd::vector<Polynomial3> polys = polynomialLoad(ps);

Spline3 spline = Spline3(ts, polys, polys, polys);

ojstd::vector<Scene> Edison2022::buildSceneGraph(const Vector2i& sceneSize) const
{
    LOG_INFO("Spline value:" << spline(0.5f).x);

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
    //auto& camera = FreeCameraController::instance();
    //Vector3f newPosition =
    // camera.heading = 0.5f / 10.f * spline(relativeSceneTime.toSeconds() * 0.5f).x;
}

ojstd::string Edison2022::getTitle() const
{
    return "Edison 2022";
}
