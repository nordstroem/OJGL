#include "FreeCameraController.h"
#include "demo/Demo.h"
#include "music/Music.h"
#include "utility/Log.h"
#include "utility/Vector.h"

namespace ojgl {
class Edison2026 final : public Demo {
public:
    ojstd::vector<Scene> buildSceneGraph(const Vector2i& sceneSize) const override;
    ojstd::string getTitle() const override;
};
}

using namespace ojgl;

namespace {

constexpr int cNumStringHits = 3;
constexpr float cLongAgo = -1000.0f;

struct StringsHistory {
    float noteTime[cNumStringHits] { cLongAgo };
    float noteTot[cNumStringHits] { };
    float lastTot { -1.0f };

    void update(float tot, float timeSinceLast, float elapsed)
    {
        if (tot < lastTot) { // Restarted or seeked backwards.
            *this = StringsHistory();
        }
        if (tot == lastTot)
            return;

        for (int i = cNumStringHits - 1; i > 0; i--) {
            noteTime[i] = noteTime[i - 1];
            noteTot[i] = noteTot[i - 1];
        }
        noteTime[0] = elapsed - timeSinceLast;
        noteTot[0] = tot;
        lastTot = tot;
    }
};

StringsHistory gStringsHistory;

}

ojstd::vector<Scene> Edison2026::buildSceneGraph(const Vector2i& sceneSize) const
{
    ojstd::vector<Scene> scenes;
    FreeCameraController::instance().set(Vector3f{-15.4204f, 11.7f, 19.5775f}, -0.632f, -0.374f);

    auto buildScene = [&sceneSize](int sceneIndex, const Duration& duration) -> Scene {
        auto cube = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2026/cube.fs");
        cube->setDefines({ { "SCENE", ojstd::to_string(sceneIndex) },
            { "NUM_STRING_HITS", ojstd::to_string(cNumStringHits) } });
        cube->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<UniformMatrix4fv>("iCameraMatrix", FreeCameraController::instance().getCameraMatrix()));
            auto music = Music::instance();
            if (music != nullptr) {
                vector.push_back(ojstd::make_shared<Uniform1f>("mBassdrum", music->syncChannels()[1].getTimeSinceAnyNote().toSeconds()));
                vector.push_back(ojstd::make_shared<Uniform1f>("mBassdrumTot", (float)music->syncChannels()[1].getTotalHits()));
                vector.push_back(ojstd::make_shared<Uniform1f>("mHihat", music->syncChannels()[2].getTimeSinceAnyNote().toSeconds()));
                vector.push_back(ojstd::make_shared<Uniform1f>("mHihatTot", (float)music->syncChannels()[2].getTotalHits()));
                vector.push_back(ojstd::make_shared<Uniform1f>("mSnare", music->syncChannels()[3].getTimeSinceAnyNote().toSeconds()));
                vector.push_back(ojstd::make_shared<Uniform1f>("mSnareTot", (float)music->syncChannels()[3].getTotalHits()));
                // introstrings is spread across channels 4-7 (one per chord voice); take the soonest hit and sum the hit counts.
                float stringsSince = ojstd::min(ojstd::min(
                    ojstd::min(music->syncChannels()[4].getTimeSinceAnyNote().toSeconds(), music->syncChannels()[5].getTimeSinceAnyNote().toSeconds()),
                    ojstd::min(music->syncChannels()[6].getTimeSinceAnyNote().toSeconds(), music->syncChannels()[7].getTimeSinceAnyNote().toSeconds())),
                    music->syncChannels()[8].getTimeSinceAnyNote().toSeconds());
                vector.push_back(ojstd::make_shared<Uniform1f>("mStrings", stringsSince));
                float stringsTot = (float)(music->syncChannels()[4].getTotalHits() + music->syncChannels()[5].getTotalHits()
                    + music->syncChannels()[6].getTotalHits() + music->syncChannels()[7].getTotalHits() + music->syncChannels()[8].getTotalHits());
                vector.push_back(ojstd::make_shared<Uniform1f>("mStringsTot", stringsTot));

                float elapsed = music->elapsedTime().toSeconds();
                gStringsHistory.update(stringsTot, stringsSince, elapsed);
                ojstd::vector<float> hitAges;
                ojstd::vector<float> hitTots;
                for (int i = 0; i < cNumStringHits; i++) {
                    hitAges.push_back(ojstd::abs(elapsed - gStringsHistory.noteTime[i]));
                    hitTots.push_back(gStringsHistory.noteTot[i]);
                }
                vector.push_back(ojstd::make_shared<Uniform1fv>("mStringsHitAge", hitAges));
                vector.push_back(ojstd::make_shared<Uniform1fv>("mStringsHitTot", hitTots));
            }
            return vector;
        });

        auto blur1 = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2026/blur1.fs");
        blur1->setInputs(cube);
        blur1->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) -> Buffer::UniformVector {
            return { ojstd::make_shared<Uniform2f>("blurDir", 1.f, 0.f) };
        });

        auto blur2 = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "edison2026/blur1.fs");
        blur2->setInputs(blur1);
        blur2->setUniformCallback([]([[maybe_unused]] float relativeSceneTime) -> Buffer::UniformVector {
            return { ojstd::make_shared<Uniform2f>("blurDir", 0.f, 1.f) };
        });

        return Scene(blur2, duration, "cube" + ojstd::to_string(sceneIndex));
    };

    scenes.push_back(buildScene(0, Duration::seconds(20)));
    scenes.push_back(buildScene(1, Duration::milliseconds(18700)));
    scenes.push_back(buildScene(2, Duration::seconds(15 + 25 + 4)));
    scenes.push_back(buildScene(3, Duration::seconds(10000)));

    return scenes;
}

ojstd::string Edison2026::getTitle() const
{
    return "Edison 2026";
}

ojstd::shared_ptr<Demo> ojgl::createSelectedDemo()
{
    return ojstd::make_shared<Edison2026>();
}
