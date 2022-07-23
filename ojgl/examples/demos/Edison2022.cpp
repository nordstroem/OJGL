#include "Edison2022.h"
#include "FreeCameraController.h"
#include "TextRenderer.hpp"
#include "music/Music.h"
#include "utility/Log.h"

using namespace ojgl;

namespace {
inline Vector3f lerp3f(const Vector3f& left, const Vector3f& right, float amount)
{
    return left * (1.f - amount) + right * amount;
}
}

static const unsigned char song[] = {
#include "songs/edison_2022_song.inc"
};

const unsigned char* Edison2022::getSong() const
{
    return song;
}

Edison2022::Edison2022()
{
}

ojstd::vector<Scene> Edison2022::buildSceneGraph(const Vector2i& sceneSize) const
{
    ojstd::vector<Scene> scenes;
    {
        auto raymarch = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2022/ship.fs");
        raymarch->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<UniformMatrix4fv>("iCameraMatrix", FreeCameraController::instance().getCameraMatrix()));

            auto music = Music::instance();

            vector.push_back(ojstd::make_shared<Uniform1f>("C_2_S", music->syncChannels()[2].getTimeSinceAnyNote().toSeconds()));
            vector.push_back(ojstd::make_shared<Uniform1f>("C_3_S", music->syncChannels()[3].getTimeSinceAnyNote().toSeconds()));
            vector.push_back(ojstd::make_shared<Uniform1f>("C_4_S", music->syncChannels()[4].getTimeSinceAnyNote().toSeconds()));
            vector.push_back(ojstd::make_shared<Uniform1f>("C_4_T", static_cast<float>(music->syncChannels()[4].getTotalHits())));
            vector.push_back(ojstd::make_shared<Uniform1f>("C_8_S", music->syncChannels()[8].getTimeSinceAnyNote().toSeconds()));
            vector.push_back(ojstd::make_shared<Uniform1f>("C_9_S", music->syncChannels()[9].getTimeSinceAnyNote().toSeconds()));
            vector.push_back(ojstd::make_shared<Uniform1f>("C_6_S", music->syncChannels()[6].getTimeSinceAnyNote().toSeconds()));
            vector.push_back(ojstd::make_shared<Uniform1f>("C_6_T", static_cast<float>(music->syncChannels()[6].getTotalHits())));

            return vector;
        });

        //auto fxaa = Buffer::construct(sceneSize.x, sceneSize.y, "common/fxaa.vs", "common/fxaa.fs");
        //fxaa->setInputs(raymarch);

        auto blur1 = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2022/blur1.fs");
        blur1->setInputs(raymarch);
        blur1->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) -> Buffer::UniformVector {
            return { ojstd::make_shared<Uniform2f>("blurDir", 1.f, 0.f) };
        });

        auto blur2 = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2022/blur1.fs");
        blur2->setInputs(blur1);
        blur2->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) -> Buffer::UniformVector {
            return { ojstd::make_shared<Uniform2f>("blurDir", 0.f, 1.f) };
        });

        auto chrom = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2022/ship_chrom_ab.fs");
        chrom->setInputs(blur2);
        chrom->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<UniformMatrix4fv>("iCameraMatrix", FreeCameraController::instance().getCameraMatrix()));

            auto music = Music::instance();

            vector.push_back(ojstd::make_shared<Uniform1f>("C_4_S", music->syncChannels()[4].getTimeSinceAnyNote().toSeconds()));

            return vector;
        });

        scenes.emplace_back(chrom, Duration::seconds(12 + 10 + 15 + 10 + 16), "sceneShip");
    }

    {
        auto raymarch = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2022/intro.fs");
        raymarch->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<UniformMatrix4fv>("iCameraMatrix", FreeCameraController::instance().getCameraMatrix()));
            return vector;
        });

        auto blur1 = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2022/blur1.fs");
        blur1->setInputs(raymarch);
        blur1->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) -> Buffer::UniformVector {
            return { ojstd::make_shared<Uniform2f>("blurDir", 1.f, 0.f) };
        });

        auto blur2 = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2022/blur1.fs");
        blur2->setInputs(blur1);
        blur2->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) -> Buffer::UniformVector {
            return { ojstd::make_shared<Uniform2f>("blurDir", 0.f, 1.f) };
        });

        auto fxaa = Buffer::construct(sceneSize.x, sceneSize.y, "common/fxaa.vs", "common/fxaa.fs");
        fxaa->setInputs(blur2);

        auto chrom = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2022/ship_chrom_ab.fs");
        chrom->setInputs(fxaa);
        chrom->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<UniformMatrix4fv>("iCameraMatrix", FreeCameraController::instance().getCameraMatrix()));

            auto music = Music::instance();

            vector.push_back(ojstd::make_shared<Uniform1f>("C_4_S", music->syncChannels()[4].getTimeSinceAnyNote().toSeconds()));

            return vector;
        });

        scenes.emplace_back(chrom, Duration::seconds(18), "scene0");
    }

    {
        auto raymarch = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2022/jellyfish.fs");
        raymarch->setUniformCallback([counter = 0.f, timeSince = 0.f]([[maybe_unused]] float relativeSceneTime) mutable {
            auto music = Music::instance();
            if (relativeSceneTime < 0.2)
                counter = static_cast<float>(music->syncChannels()[2].getTotalHits());

            timeSince = music->syncChannels()[2].getTimeSinceAnyNote().toSeconds();
            timeSince = timeSince > 5 ? 0.f : timeSince;
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<UniformMatrix4fv>("iCameraMatrix", FreeCameraController::instance().getCameraMatrix()));
            vector.push_back(ojstd::make_shared<Uniform1f>("C_2_S", timeSince));
            vector.push_back(ojstd::make_shared<Uniform1f>("C_2_T", ojstd::max(0.f, static_cast<float>(music->syncChannels()[2].getTotalHits()) - counter)));
            return vector;
        });

        auto blur1 = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2022/blur1.fs");
        blur1->setInputs(raymarch);
        blur1->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) -> Buffer::UniformVector {
            return { ojstd::make_shared<Uniform2f>("blurDir", 1.f, 0.f) };
        });

        auto blur2 = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2022/blur1.fs");
        blur2->setInputs(blur1);
        blur2->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) -> Buffer::UniformVector {
            return { ojstd::make_shared<Uniform2f>("blurDir", 0.f, 1.f) };
        });

        auto fxaa = Buffer::construct(sceneSize.x, sceneSize.y, "common/fxaa.vs", "common/fxaa.fs");
        fxaa->setInputs(blur2);

        //auto post = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2022/post.fs");
        //post->setInputs(fxaa);

        auto chrom = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2022/ship_chrom_ab.fs");
        chrom->setInputs(fxaa);
        chrom->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<UniformMatrix4fv>("iCameraMatrix", FreeCameraController::instance().getCameraMatrix()));

            auto music = Music::instance();

            vector.push_back(ojstd::make_shared<Uniform1f>("C_4_S", music->syncChannels()[4].getTimeSinceAnyNote().toSeconds()));

            return vector;
        });

        scenes.emplace_back(chrom, Duration::seconds(20), "scene1");
    }

    {
        auto raymarch = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2022/ascent.fs");
        raymarch->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<UniformMatrix4fv>("iCameraMatrix", FreeCameraController::instance().getCameraMatrix()));
            auto music = Music::instance();
            vector.push_back(ojstd::make_shared<Uniform1f>("C_4_T", static_cast<float>(music->syncChannels()[4].getTotalHits())));

            return vector;
        });

        auto fxaa = Buffer::construct(sceneSize.x, sceneSize.y, "common/fxaa.vs", "common/fxaa.fs");
        fxaa->setInputs(raymarch);

        auto chrom = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2022/ship_chrom_ab.fs");
        chrom->setInputs(fxaa);
        chrom->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) mutable {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<UniformMatrix4fv>("iCameraMatrix", FreeCameraController::instance().getCameraMatrix()));

            auto music = Music::instance();

            vector.push_back(ojstd::make_shared<Uniform1f>("C_4_S", music->syncChannels()[4].getTimeSinceAnyNote().toSeconds()));

            return vector;
        });

        scenes.emplace_back(chrom, Duration::seconds(34), "scene2");
    }

    {
        auto raymarch = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2022/grass.fs");
        raymarch->setUniformCallback([counter = 0.f, timeSince = 0.f]([[maybe_unused]] float relativeSceneTime) mutable {
            auto music = Music::instance();
            if (relativeSceneTime < 1)
                counter = static_cast<float>(music->syncChannels()[6].getTotalHits());

            timeSince = music->syncChannels()[6].getTimeSinceAnyNote().toSeconds();
            timeSince = timeSince > 1 ? 0.f : timeSince;

            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<UniformMatrix4fv>("iCameraMatrix", FreeCameraController::instance().getCameraMatrix()));
            vector.push_back(ojstd::make_shared<Uniform1f>("C_6_S", timeSince));
            vector.push_back(ojstd::make_shared<Uniform1f>("C_6_T", ojstd::max(0.f, static_cast<float>(music->syncChannels()[6].getTotalHits()) - counter)));
            vector.push_back(ojstd::make_shared<Uniform1f>("C_4_S", music->syncChannels()[2].getTimeSinceAnyNote().toSeconds()));
            return vector;
        });

        auto fxaa = Buffer::construct(sceneSize.x, sceneSize.y, "common/fxaa.vs", "common/fxaa.fs");
        fxaa->setInputs(raymarch);

        auto chrom = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2022/ship_chrom_ab.fs");
        chrom->setInputs(fxaa);
        chrom->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<UniformMatrix4fv>("iCameraMatrix", FreeCameraController::instance().getCameraMatrix()));

            auto music = Music::instance();

            vector.push_back(ojstd::make_shared<Uniform1f>("C_4_S", music->syncChannels()[2].getTimeSinceAnyNote().toSeconds()));

            return vector;
        });

        scenes.emplace_back(chrom, Duration::seconds(32 + 2), "scene3");
    }
    return scenes;
}

void Edison2022::update(const Duration& relativeSceneTime, const Duration& elapsedTime, const ojstd::string& currentScene) const
{
    OJ_UNUSED(elapsedTime);
    OJ_UNUSED(relativeSceneTime);
    if (currentScene == "scene0") {
        FreeCameraController::instance().set({ 85.5f, 81.9f, -63.f }, -4.0f, -0.674f);
    } else if (currentScene == "scene1") {
        if (relativeSceneTime.toSeconds() > 11) {
            FreeCameraController::instance().set({ 30.17f, 23.19f, 34.3f }, 2.548f, -0.374f);
        } else {
            FreeCameraController::instance().set({ 77.832f, 90.95f, -63.8f }, 2.35f, -0.054f);
        }
    } else if (currentScene == "scene2") {
        FreeCameraController::instance().set({ 31.64f, 72.6f, -23.58f }, { 0.f, 0.f, 0.f });
    } else if (currentScene == "scene3") {
        const float t = ojstd::smoothstep(5, 15, relativeSceneTime.toSeconds());
        /*const Vector3f start = { 31.64f, 62.6f, -23.58f };
        const Vector3f end = { 51.94f, 61.16f, -46.8f };
        FreeCameraController::instance().set(ojstd::lerp(start, end, t), ojstd::lerp(2.272f, 2.38f, t), ojstd::lerp(-1.066f, -0.646f, t));*/
        const Vector3f start = { 31.64f, 72.6f, -23.58f };
        const Vector3f end = { 81.64f, 82.f, -23.58f };
        const Vector3f startTarget = { 0.f, 0.f, 0.f };
        const Vector3f endTarget = { 0.f, 10.f, 0.f };

        const Vector3f split1 = lerp3f(start, end, t);
        const Vector3f split1Target = lerp3f(startTarget, endTarget, t);

        if (relativeSceneTime.toSeconds() <= 15) {
            FreeCameraController::instance().set(split1, split1Target);
        } else {
            float t2 = ojstd::smoothstep(15.f, 25.f, relativeSceneTime.toSeconds());
            const Vector3f split2Target = { 0.f, 10.f, 0.0f };
            Vector3f split2 = { 0.2f, 100.1f, -0.2f };
            split2 = lerp3f(end, split2, t2 * 0.95f);
            FreeCameraController::instance().set(split2, split1Target);
        }
    }
}

//(42.3825, 62.6, -30.457), [ 1.172, -0.166 ]

ojstd::string Edison2022::getTitle() const
{
    return "Lake Lurker";
}