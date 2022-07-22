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

ojstd::shared_ptr<Texture> Edison2022::getText(const ojstd::string& text) const
{
    if (!this->_textures.contains(text))
        this->_textures[text] = TextRenderer::instance().get(text);

    return this->_textures[text];
}

Edison2022::Edison2022()
{
    // FreeCameraController::instance().set({ 39.0531f, 50.1299f, 20.5951f }, 1.114f, -0.846f);

    //FreeCameraController::instance().set({ 0, 0, 30 }, { 0.f, 0.f, 0.f });
    //FreeCameraController::instance().set({ 85.5, 81.9, -63 }, -4.0f, -0.674f);

    // experiment.fs
    FreeCameraController::instance().set({ 30.17f, 23.19f, 34.3f }, 2.548f, -0.374f);
    FreeCameraController::instance().set({ 31.64f, 62.6f, -23.58f }, 2.272f, -1.066f);
    FreeCameraController::instance().set({ 81.64f, 82.f, -23.58f }, { 0.f, 10.f, 0.f });

    // ascent.fs
    //FreeCameraController::instance().set({ 0.f, 85.5f, 90.74f }, 0.f, -0.77f);
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

            vector.push_back(ojstd::make_shared<Uniform1f>("C_2_SINCE", music->syncChannels()[2].getTimeSinceAnyNote().toSeconds()));
            vector.push_back(ojstd::make_shared<Uniform1f>("C_3_SINCE", music->syncChannels()[3].getTimeSinceAnyNote().toSeconds()));
            vector.push_back(ojstd::make_shared<Uniform1f>("C_4_SINCE", music->syncChannels()[4].getTimeSinceAnyNote().toSeconds()));
            vector.push_back(ojstd::make_shared<Uniform1f>("C_4_TOTAL", music->syncChannels()[4].getTotalHits()));
            vector.push_back(ojstd::make_shared<Uniform1f>("C_8_SINCE", music->syncChannels()[8].getTimeSinceAnyNote().toSeconds()));
            vector.push_back(ojstd::make_shared<Uniform1f>("C_9_SINCE", music->syncChannels()[9].getTimeSinceAnyNote().toSeconds()));
            vector.push_back(ojstd::make_shared<Uniform1f>("C_6_SINCE", music->syncChannels()[6].getTimeSinceAnyNote().toSeconds()));
            vector.push_back(ojstd::make_shared<Uniform1f>("C_6_TOTAL", music->syncChannels()[6].getTotalHits()));

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

            vector.push_back(ojstd::make_shared<Uniform1f>("CHANNEL_4_SINCE", music->syncChannels()[4].getTimeSinceAnyNote().toSeconds()));

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

        raymarch->setTextureCallback([this]([[maybe_unused]] float relativeSceneTime) {
            ojstd::vector<ojstd::shared_ptr<Uniform1t>> vector;
            vector.push_back(ojstd::make_shared<Uniform1t>("ojText", this->getText("O J")));
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

            vector.push_back(ojstd::make_shared<Uniform1f>("CHANNEL_4_SINCE", music->syncChannels()[4].getTimeSinceAnyNote().toSeconds()));

            return vector;
        });

        scenes.emplace_back(chrom, Duration::seconds(18), "scene0");
    }

    {
        auto raymarch = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2022/jellyfish.fs");
        raymarch->setUniformCallback([counter = 0.f, timeSince = 0.f]([[maybe_unused]] float relativeSceneTime) mutable {
            auto music = Music::instance();
            if (relativeSceneTime < 0.2)
                counter = music->syncChannels()[2].getTotalHits();

            timeSince = music->syncChannels()[2].getTimeSinceAnyNote().toSeconds();
            timeSince = timeSince > 5 ? 0.f : timeSince;
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<UniformMatrix4fv>("iCameraMatrix", FreeCameraController::instance().getCameraMatrix()));
            vector.push_back(ojstd::make_shared<Uniform1f>("C_2_SINCE", timeSince));
            vector.push_back(ojstd::make_shared<Uniform1f>("C_2_TOTAL", ojstd::max(0.f, music->syncChannels()[2].getTotalHits() - counter)));
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

        /*auto fxaa = Buffer::construct(sceneSize.x, sceneSize.y, "common/fxaa.vs", "common/fxaa.fs");
        fxaa->setInputs(blur2);*/

        //auto post = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2022/post.fs");
        //post->setInputs(fxaa);

        auto chrom = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2022/ship_chrom_ab.fs");
        chrom->setInputs(blur2);
        chrom->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<UniformMatrix4fv>("iCameraMatrix", FreeCameraController::instance().getCameraMatrix()));

            auto music = Music::instance();

            vector.push_back(ojstd::make_shared<Uniform1f>("CHANNEL_4_SINCE", music->syncChannels()[4].getTimeSinceAnyNote().toSeconds()));

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
            vector.push_back(ojstd::make_shared<Uniform1f>("C_4_TOTAL", music->syncChannels()[4].getTotalHits()));

            return vector;
        });

        /*auto fxaa = Buffer::construct(sceneSize.x, sceneSize.y, "common/fxaa.vs", "common/fxaa.fs");
        fxaa->setInputs(raymarch);*/

        auto chrom = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2022/ship_chrom_ab.fs");
        chrom->setInputs(raymarch);
        chrom->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) mutable {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<UniformMatrix4fv>("iCameraMatrix", FreeCameraController::instance().getCameraMatrix()));

            auto music = Music::instance();

            vector.push_back(ojstd::make_shared<Uniform1f>("CHANNEL_4_SINCE", music->syncChannels()[4].getTimeSinceAnyNote().toSeconds()));

            return vector;
        });

        scenes.emplace_back(chrom, Duration::seconds(34), "scene2");
    }

    {
        auto raymarch = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2022/grass.fs");
        raymarch->setUniformCallback([counter = 0.f, timeSince = 0.f]([[maybe_unused]] float relativeSceneTime) mutable {
            auto music = Music::instance();
            if (relativeSceneTime < 1)
                counter = music->syncChannels()[6].getTotalHits();

            timeSince = music->syncChannels()[6].getTimeSinceAnyNote().toSeconds();
            timeSince = timeSince > 1 ? 0.f : timeSince;

            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<UniformMatrix4fv>("iCameraMatrix", FreeCameraController::instance().getCameraMatrix()));
            vector.push_back(ojstd::make_shared<Uniform1f>("C_6_SINCE", timeSince));
            vector.push_back(ojstd::make_shared<Uniform1f>("C_6_TOTAL", ojstd::max(0.f, music->syncChannels()[6].getTotalHits() - counter)));
            vector.push_back(ojstd::make_shared<Uniform1f>("C_4_SINCE", music->syncChannels()[2].getTimeSinceAnyNote().toSeconds()));
            return vector;
        });

        /*auto fxaa = Buffer::construct(sceneSize.x, sceneSize.y, "common/fxaa.vs", "common/fxaa.fs");
        fxaa->setInputs(raymarch);*/

        auto chrom = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2022/ship_chrom_ab.fs");
        chrom->setInputs(raymarch);
        chrom->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<UniformMatrix4fv>("iCameraMatrix", FreeCameraController::instance().getCameraMatrix()));

            auto music = Music::instance();

            vector.push_back(ojstd::make_shared<Uniform1f>("CHANNEL_4_SINCE", music->syncChannels()[2].getTimeSinceAnyNote().toSeconds()));

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
    auto& camera = FreeCameraController::instance();
    if (currentScene == "scene0") {
        FreeCameraController::instance().set({ 85.5, 81.9, -63 }, -4.0f, -0.674f);
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
            split2 = lerp3f(end, split2, t2 * 0.95);
            FreeCameraController::instance().set(split2, split1Target);
        }
    }
}

//(42.3825, 62.6, -30.457), [ 1.172, -0.166 ]

ojstd::string Edison2022::getTitle() const
{
    return "Edison 2022";
}
