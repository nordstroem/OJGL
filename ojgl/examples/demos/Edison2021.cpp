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
    return "OJ - Edison 2021";
}

const unsigned char* Edison2021::getSong() const
{
    return nullptr;
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

        auto fade = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "common/fade.fs");
        fade->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<Uniform1f>("startFadeTime", 0.f));
            vector.push_back(ojstd::make_shared<Uniform1f>("endFadeTime", 5.f));
            vector.push_back(ojstd::make_shared<Uniform1b>("fadeIn", true));
            return vector;
        });

        fade->setInputs(fxaa);
        scenes.emplace_back(fade, Duration::seconds(30), "raymarchScene");

        introText->setTextureCallback([this]([[maybe_unused]] float relativeSceneTime) {
            ojstd::vector<ojstd::shared_ptr<Uniform1t>> vector;
            vector.push_back(ojstd::make_shared<Uniform1t>("moltresTexture", this->getText("Moltres")));
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
            return vector;
        });

        auto fxaa = Buffer::construct(sceneSize.x, sceneSize.y, "common/fxaa.vs", "common/fxaa.fs");
        fxaa->setInputs(blob);

        auto fade = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "common/fade.fs");
        fade->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<Uniform1f>("startFadeTime", 19.5f));
            vector.push_back(ojstd::make_shared<Uniform1f>("endFadeTime", 20.f));
            vector.push_back(ojstd::make_shared<Uniform1b>("fadeIn", false));
            return vector;
        });

        fade->setInputs(fxaa);
        scenes.emplace_back(fade, Duration::seconds(20), "blobScene");
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

        auto fade = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "common/fade.fs");
        fade->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<Uniform1f>("startFadeTime", 0.0f));
            vector.push_back(ojstd::make_shared<Uniform1f>("endFadeTime", 0.5f));
            vector.push_back(ojstd::make_shared<Uniform1b>("fadeIn", true));
            return vector;
        });
        fade->setInputs(fxaa);

        scenes.emplace_back(fade, Duration::seconds(20), "tunnelTransformScene");
    }

    {
        auto raymarch = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2021/pipes.fs");
        raymarch->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<UniformMatrix4fv>("iCameraMatrix", FreeCameraController::instance().getCameraMatrix()));
            return vector;
        });

        auto fxaa = Buffer::construct(sceneSize.x, sceneSize.y, "common/fxaa.vs", "common/fxaa.fs");
        fxaa->setInputs(raymarch);

        scenes.emplace_back(fxaa, Duration::seconds(31), "pipesScene");
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

        scenes.emplace_back(radialBlur, Duration::seconds(99999), "planeScene");
    }

    return scenes;
}