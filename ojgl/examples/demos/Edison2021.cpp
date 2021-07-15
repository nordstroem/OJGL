#include "Edison2021.h"
#include "FreeCameraController.h"
#include "TextRenderer.hpp"
#include "music/Music.h"

using namespace ojgl;

Edison2021::Edison2021()
{
}

ojstd::string Edison2021::getTitle() const
{
    return "OJ - Enigma";
}

static const unsigned char song[] = {
#include "songs/edison_2021_song.inc"
};

const unsigned char* Edison2021::getSong() const
{
    return song;
}

ojstd::shared_ptr<Texture> Edison2021::getText(const ojstd::string& text) const
{
    if (!this->_textures.contains(text))
        this->_textures[text] = TextRenderer::instance().get(text);

    return this->_textures[text];
}

ojstd::vector<Scene> Edison2021::buildSceneGraph(const Vector2i& sceneSize) const
{
    ojstd::vector<Scene> scenes;

    {
        auto introText = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2021/introText.fs");
        introText->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<UniformMatrix4fv>("iCameraMatrix", FreeCameraController::instance().getCameraMatrix()));
            return vector;
        });

        auto fxaa = Buffer::construct(sceneSize.x, sceneSize.y, "common/fxaa.vs", "common/fxaa.fs");
        fxaa->setInputs(introText);

        auto chrom = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2021/chrom_ab.fs");
        chrom->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<Uniform1f>("drumSince", Music::instance()->syncChannels()[3].getTimeSinceLast(0).toSeconds()));
            return vector;
        });

        chrom->setInputs(fxaa);

        auto fade = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "common/fade.fs");
        fade->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<Uniform1f>("startFadeTime", 0.f));
            vector.push_back(ojstd::make_shared<Uniform1f>("endFadeTime", 10.f));
            vector.push_back(ojstd::make_shared<Uniform1b>("fadeIn", true));
            return vector;
        });

        fade->setInputs(chrom);

        scenes.emplace_back(chrom, Duration::seconds(29), "raymarchScene");

        introText->setTextureCallback([this]([[maybe_unused]] float relativeSceneTime) {
            ojstd::vector<ojstd::shared_ptr<Uniform1t>> vector;
            vector.push_back(ojstd::make_shared<Uniform1t>("moltresTexture", this->getText("Enigma")));
            vector.push_back(ojstd::make_shared<Uniform1t>("ojTexture", this->getText("by OJ")));
            vector.push_back(ojstd::make_shared<Uniform1t>("edisonTexture", this->getText("For Edison 2021")));
            return vector;
        });
    }

    {
        auto blob = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2021/blob.fs");
        blob->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<UniformMatrix4fv>("iCameraMatrix", FreeCameraController::instance().getCameraMatrix()));
            vector.push_back(ojstd::make_shared<Uniform1f>("totalHits", Music::instance()->syncChannels()[3].getTotalHits()));
            return vector;
        });

        auto fxaa = Buffer::construct(sceneSize.x, sceneSize.y, "common/fxaa.vs", "common/fxaa.fs");
        fxaa->setInputs(blob);

        auto chrom = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2021/chrom_ab.fs");
        chrom->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<Uniform1f>("drumSince", Music::instance()->syncChannels()[3].getTimeSinceLast(0).toSeconds()));
            return vector;
        });

        chrom->setInputs(fxaa);

        scenes.emplace_back(chrom, Duration::seconds(26), "blobScene");
    }

    {
        auto tunnelTransform = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2021/tunnel_transform.fs");
        tunnelTransform->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<UniformMatrix4fv>("iCameraMatrix", FreeCameraController::instance().getCameraMatrix()));
            return vector;
        });

        auto fxaa = Buffer::construct(sceneSize.x, sceneSize.y, "common/fxaa.vs", "common/fxaa.fs");
        fxaa->setInputs(tunnelTransform);

        auto chrom = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2021/chrom_ab.fs");
        chrom->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<Uniform1f>("drumSince", Music::instance()->syncChannels()[3].getTimeSinceLast(0).toSeconds()));
            return vector;
        });

        chrom->setInputs(fxaa);

        scenes.emplace_back(chrom, Duration::milliseconds(26200), "tunnelTransformScene");
    }

    {
        auto raymarch = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2021/pipes.fs");
        raymarch->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<UniformMatrix4fv>("iCameraMatrix", FreeCameraController::instance().getCameraMatrix()));
            ojstd::vector<float> ch1;
            for (int i = 0; i < 15; i++) {
                ch1.push_back(Music::instance()->syncChannels()[1].getTimeSinceLast(i).toSeconds());
            }
            vector.push_back(ojstd::make_shared<Uniform1fv>("channel1", ch1));
            //vector.push_back(ojstd::make_shared<Uniform1fv>("channel1", Music::instance()->syncChannels()[1].getTimeSinceLast(0).toSeconds()));
            vector.push_back(ojstd::make_shared<Uniform1f>("drum", Music::instance()->syncChannels()[3].getTimeSinceLast(0).toSeconds()));
            return vector;
        });

        auto fxaa = Buffer::construct(sceneSize.x, sceneSize.y, "common/fxaa.vs", "common/fxaa.fs");
        fxaa->setInputs(raymarch);

        auto chrom = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2021/chrom_ab.fs");
        chrom->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<Uniform1f>("drumSince", Music::instance()->syncChannels()[3].getTimeSinceLast(0).toSeconds()));
            return vector;
        });

        chrom->setInputs(fxaa);

        scenes.emplace_back(chrom, Duration::seconds(31), "pipesScene");
    }

    {
        auto raymarch = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2021/plane.fs");
        raymarch->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<UniformMatrix4fv>("iCameraMatrix", FreeCameraController::instance().getCameraMatrix()));
            return vector;
        });

        auto radialBlur = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "common/radial_blur.fs");
        radialBlur->setInputs(raymarch);

        auto chrom = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2021/chrom_ab.fs");
        chrom->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<Uniform1f>("drumSince", Music::instance()->syncChannels()[3].getTimeSinceLast(0).toSeconds()));
            return vector;
        });

        chrom->setInputs(radialBlur);

        auto fade = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "common/fade.fs");
        fade->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<Uniform1f>("startFadeTime", 18.f));
            vector.push_back(ojstd::make_shared<Uniform1f>("endFadeTime", 25.f));
            vector.push_back(ojstd::make_shared<Uniform1b>("fadeIn", false));
            return vector;
        });

        fade->setInputs(chrom);

        scenes.emplace_back(fade, Duration::seconds(25), "planeScene");
    }

    return scenes;
}
