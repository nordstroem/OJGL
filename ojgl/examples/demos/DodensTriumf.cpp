#include "DodensTriumf.h"
#include "music/Music.h"

using namespace ojgl;

ojstd::string DodensTriumf::getTitle() const
{
    return "OJ - Dödens Triumf";
}

static const unsigned char song[] = {
#include "songs/dodenstriumf_song.inc"
};

const unsigned char* DodensTriumf::getSong() const
{
    return song;
}

ojstd::vector<Scene> DodensTriumf::buildSceneGraph(const Vector2i& sceneSize) const
{
    ojstd::vector<Scene> scenes;

    {
        auto introScene = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "dodenstriumf/introScene.fs");
        scenes.emplace_back(introScene, Duration::seconds(7));
    }

    {
        auto graveScene = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "dodenstriumf/graveScene.fs");
        graveScene->setUniformCallback([](float relativeSceneTime) {
          Buffer::UniformVector vector;

          auto music = Music::instance();
          vector.push_back(ojstd::make_shared<Uniform1f>("CHANNEL_11_SINCE", music->syncChannels()[11].getTimeSinceLast(0).toSeconds()));
          vector.push_back(ojstd::make_shared<Uniform1f>("CHANNEL_11_TOTAL", music->syncChannels()[11].getTotalHits()));
          ojstd::vector<float> since;
          since.push_back(music->syncChannels()[4].getTimeSinceLast(0).toSeconds());
          since.push_back(music->syncChannels()[4].getTimeSinceLast(1).toSeconds());
          vector.push_back(ojstd::make_shared<Uniform1fv>("CHANNEL_4_SINCE", since));
          return vector;
        });

        auto graveFxaa = Buffer::construct(sceneSize.x, sceneSize.y, "common/fxaa.vs", "common/fxaa.fs");
        graveFxaa->setInputs(graveScene);
        auto graveScenePost = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "dodenstriumf/graveScenePost.fs");
        graveScenePost->setInputs(graveFxaa);
        graveScenePost->setUniformCallback([](float relativeSceneTime) {
            Buffer::UniformVector vector;

            auto music = Music::instance();
            float m = ojstd::min(music->syncChannels()[4].getTimeToNext(0).toSeconds(), music->syncChannels()[4].getTimeToNext(1).toSeconds());
            vector.push_back(ojstd::make_shared<Uniform1f>("CHANNEL_4_TO", m));
            vector.push_back(ojstd::make_shared<Uniform1f>("CHANNEL_4_TOTAL", music->syncChannels()[4].getTotalHitsPerNote(0) + music->syncChannels()[4].getTotalHitsPerNote(1)));
            vector.push_back(ojstd::make_shared<Uniform1f>("CHANNEL_11_SINCE", music->syncChannels()[11].getTimeSinceLast(0).toSeconds()));
            return vector;
        });

        scenes.emplace_back(graveScenePost, Duration::milliseconds(22500 + 15000 + 15000 + 10000));
    }

    {
        auto roomScene = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "dodenstriumf/roomScene.fs");
        auto roomFxaa = Buffer::construct(sceneSize.x, sceneSize.y, "common/fxaa.vs", "common/fxaa.fs");
        roomFxaa->setInputs(roomScene);
        auto roomScenePost = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "dodenstriumf/roomScenePost.fs");
        roomScenePost->setInputs(roomFxaa);
        scenes.emplace_back(roomScenePost, Duration::seconds(42));
    }

    return scenes;
}