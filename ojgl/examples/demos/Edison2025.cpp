#include "Edison2025.h"
#include "FreeCameraController.h"
#include "TextRenderer.hpp"

namespace ojgl {

Edison2025::Edison2025()
{
}

ojstd::shared_ptr<Texture> Edison2025::getText(const ojstd::string& text, const ojstd::string& font) const
{
    if (!this->_textures.contains(text + font))
        this->_textures[text + font] = TextRenderer::instance().get(text, font);

    return this->_textures[text + font];
}

ojstd::vector<Scene> Edison2025::buildSceneGraph(const Vector2i& sceneSize) const
{
    ojstd::vector<Scene> scenes;
    auto noise = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2025/noise.fs");
    noise->setRenderOnce(true);

    auto lissajous = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2025/lissajous.fs");
    lissajous->setFeedbackInputs(lissajous);

    auto radar = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2025/radar.fs");
    radar->setTextureCallback([this]([[maybe_unused]] float relativeSceneTime) {
        ojstd::vector<ojstd::shared_ptr<Uniform1t>> vector;
        vector.push_back(ojstd::make_shared<Uniform1t>("ojTexture", this->getText("OJ", "Arial Black")));
        return vector;
    });
    radar->setFeedbackInputs(radar);

    auto experiment = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2025/experiment.fs");
    experiment->setInputs(noise, lissajous, radar);

    experiment->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
        Buffer::UniformVector vector;
        vector.push_back(ojstd::make_shared<UniformMatrix4fv>("iCameraMatrix", FreeCameraController::instance().getCameraMatrix()));
        return vector;
    });

    experiment->setTextureCallback([this]([[maybe_unused]] float relativeSceneTime) {
        ojstd::vector<ojstd::shared_ptr<Uniform1t>> vector;
        vector.push_back(ojstd::make_shared<Uniform1t>("borgilaTexture", this->getText("BORGILA", "Arial Black")));
        return vector;
    });

    scenes.emplace_back(experiment, Duration::seconds(1000000), "experiment");
    return scenes;
}

const unsigned char* Edison2025::getSong() const { return nullptr; }

ojstd::string Edison2025::getTitle() const
{
    return "Edison 2025";
}

void Edison2025::update(const Duration& relativeSceneTime, const Duration& elapsedTime, const ojstd::string& currentScene) const
{
    OJ_UNUSED(relativeSceneTime);
    OJ_UNUSED(elapsedTime);
    OJ_UNUSED(currentScene);
}

}
