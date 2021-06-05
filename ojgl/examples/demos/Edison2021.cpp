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

ojstd::vector<Scene> Edison2021::buildSceneGraph(const Vector2i& sceneSize) const
{
    ojstd::vector<Scene> scenes;
    {
        auto raymarch = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2021/tunnel.fs");
        raymarch->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<UniformMatrix4fv>("iCameraMatrix", FreeCameraController::instance().getCameraMatrix()));
            return vector;
        });

        auto fxaa = Buffer::construct(sceneSize.x, sceneSize.y, "common/fxaa.vs", "common/fxaa.fs");
        fxaa->setInputs(raymarch);

        scenes.emplace_back(fxaa, Duration::seconds(9999), "raymarchScene");

        raymarch->setTextureCallback([this]([[maybe_unused]] float relativeSceneTime) {
            if (this->_textTexture == nullptr) {
                this->_textTexture = TextRenderer::instance().get("Edison 2021");
            }
            ojstd::vector<ojstd::shared_ptr<Uniform1t>> vector;
            vector.push_back(ojstd::make_shared<Uniform1t>("textTexture", _textTexture));
            return vector;
        });
    }

    return scenes;
}