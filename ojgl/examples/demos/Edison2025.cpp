#include "Edison2025.h"
#include "FreeCameraController.h"
#include "TextRenderer.hpp"

namespace ojgl {

namespace {
    float previousTimes[] = { 0.0f, 0.0f };
    int previousTimeIndex = 0;
}

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

    // Borgila scene
    {
        auto noise = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2025/noise.fs");
        noise->setRenderOnce(true);

        // lissajous
        auto lissajous = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2025/lissajous.fs");
        lissajous->setFeedbackInputs(lissajous);

        lissajous->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<Uniform1f>("iPreviousTime", previousTimes[previousTimeIndex]));
            return vector;
        });

        // radar
        auto radar = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2025/radar.fs");
        radar->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<Uniform1f>("iPreviousTime", previousTimes[previousTimeIndex]));
            return vector;
        });
        radar->setFeedbackInputs(radar);

        // oj text
        auto ojText = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2025/oj_text.fs");
        ojText->setFeedbackInputs(ojText);
        ojText->setTextureCallback([this]([[maybe_unused]] float relativeSceneTime) {
            ojstd::vector<ojstd::shared_ptr<Uniform1t>> vector;
            vector.push_back(ojstd::make_shared<Uniform1t>("ojTexture", this->getText("OJ", "Arial Black")));
            return vector;
        });

        ojText->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<Uniform1f>("iPreviousTime", previousTimes[previousTimeIndex]));
            return vector;
        });

        auto experiment = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2025/borgila.fs");
        experiment->setInputs(noise, lissajous, radar, ojText);

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

        scenes.emplace_back(experiment, Duration::seconds(20), "borgila");
    }

    // indoor scene
    {
        auto noise = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2025/noise.fs");
        noise->setRenderOnce(true);

        // lissajous
        auto lissajous = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2025/lissajous.fs");
        lissajous->setFeedbackInputs(lissajous);

        lissajous->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<Uniform1f>("iPreviousTime", previousTimes[previousTimeIndex]));
            return vector;
        });

        // radar
        auto radar = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2025/radar.fs");
        radar->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<Uniform1f>("iPreviousTime", previousTimes[previousTimeIndex]));
            return vector;
        });
        radar->setFeedbackInputs(radar);

        // oj text
        auto ojText = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2025/oj_text.fs");
        ojText->setFeedbackInputs(ojText);
        ojText->setTextureCallback([this]([[maybe_unused]] float relativeSceneTime) {
            ojstd::vector<ojstd::shared_ptr<Uniform1t>> vector;
            vector.push_back(ojstd::make_shared<Uniform1t>("ojTexture", this->getText("OJ", "Arial Black")));
            return vector;
        });

        ojText->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<Uniform1f>("iPreviousTime", previousTimes[previousTimeIndex]));
            return vector;
        });

        auto experiment = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2025/indoor.fs");
        experiment->setInputs(noise, lissajous, radar, ojText);

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

        scenes.emplace_back(experiment, Duration::seconds(1000000), "indoor");
    }

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
    const float currentTime = relativeSceneTime.toSeconds<float>();
    previousTimes[previousTimeIndex] = currentTime;
    previousTimeIndex++;
    previousTimeIndex = previousTimeIndex % 2;
    for (size_t i = 0; i < 2; i++) {
        previousTimes[previousTimeIndex] = ojstd::clamp(previousTimes[previousTimeIndex], currentTime - 0.5f, currentTime);
    }

    auto& camera = FreeCameraController::instance();

    if (currentScene == "borgila") {
        if (currentTime < 5.0) {
            camera.set({ 14.7328f, 7.39f, -6.54882f }, 0.708f, -0.0700001f);
        } else if (currentTime < 10.0) {
            camera.set({ 30.7056f, 38.64f, 29.3115f }, 0.772f, -0.550f);
        } else if (currentTime < 20.0) {
            camera.set({ -19.6426f, 3.28002f, -2.09492f }, 0.676f, -0.08f);
        }
    } else {
        if (currentTime < 30.0) {
            Vector3f cameraPosition { 21.9963f, 3.94f, -72.8188f };
            float speed = 1.0f;
            float heading = -3.1415f;
            cameraPosition.x += speed * ojstd::sin(heading) * currentTime;
            cameraPosition.z += -speed * ojstd::cos(heading) * currentTime;

            camera.set(cameraPosition, heading, -0.22f);
        }
    }
}

}
