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
auto ts = {0.0f, 10.0f, 15.0f, 20.0f, };
float ps_x[] = {39.0531f, -1.4210854715202005e-15f, -0.38966699999999976f, 0.041008399999999987f, 41.0948f, 4.509180000000001f, 0.8405850000000001f, -0.24314980000000003f, 54.2616f, -5.321205f, -2.806662f, 0.44517100000000004f, };
float ps_y[] = {50.1299f, 1.4210854715202005e-15f, -0.15864381818181847f, 0.014754381818181833f, 49.0199f, 1.2534381818181821f, 0.2839876363636364f, -0.10453585454545455f, 49.3198f, -3.746874545454546f, -1.2840501818181815f, 0.22116501818181816f, };
float ps_z[] = {20.5951f, 0.0f, -0.708666409090909f, 0.04844675090909091f, -1.82479f, 0.3606970909090907f, 0.7447361181818182f, -0.16138878727272726f, -1.5765f, -4.296100772727272f, -1.6760956909090912f, 0.28076076909090913f, };

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
    auto& camera = FreeCameraController::instance();
    Vector3f newPosition = spline(relativeSceneTime.toSeconds());
    camera.set(newPosition, { 0.f, 0.f, 0.f });
}

ojstd::string Edison2022::getTitle() const
{
    return "Edison 2022";
}
