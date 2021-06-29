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

        scenes.emplace_back(fxaa, Duration::seconds(5), "raymarchScene");

        introText->setTextureCallback([this]([[maybe_unused]] float relativeSceneTime) {
            ojstd::vector<ojstd::shared_ptr<Uniform1t>> vector;
            vector.push_back(ojstd::make_shared<Uniform1t>("pirateyTexture", this->getText("Piratey!?")));
            return vector;
        });
    }

    {
        auto raymarch = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2021/bird.fs");
        raymarch->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<UniformMatrix4fv>("iCameraMatrix", FreeCameraController::instance().getCameraMatrix()));
            return vector;
        });

        auto radialBlur = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "common/radial_blur.fs");
        radialBlur->setInputs(raymarch);

        scenes.emplace_back(radialBlur, Duration::seconds(5), "raymarchScene");
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
        scenes.emplace_back(fxaa, Duration::seconds(5), "blobScene");
    }

    {
        auto introText = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2021/tunnel_transform.fs");
        introText->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<UniformMatrix4fv>("iCameraMatrix", FreeCameraController::instance().getCameraMatrix()));
            return vector;
        });

        auto fxaa = Buffer::construct(sceneSize.x, sceneSize.y, "common/fxaa.vs", "common/fxaa.fs");
        fxaa->setInputs(introText);

        scenes.emplace_back(fxaa, Duration::seconds(5), "tunnelTransformScene");
    }

    {
        auto wireCube = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2021/wire_cube.fs");
        wireCube->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<UniformMatrix4fv>("iCameraMatrix", FreeCameraController::instance().getCameraMatrix()));
            return vector;
        });

        auto fxaa = Buffer::construct(sceneSize.x, sceneSize.y, "common/fxaa.vs", "common/fxaa.fs");
        fxaa->setInputs(wireCube);
        scenes.emplace_back(fxaa, Duration::seconds(5), "wireCubeScene");
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

        scenes.emplace_back(fxaa, Duration::seconds(30), "pipesScene");
    }

    {
        auto raymarch = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2021/plane.fs", "#define CUSTOM_DELTA_MODIFIER\nfloat tunnelDeltaModifier() { return 1.0; };\n");
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