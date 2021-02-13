#include "Eldur.h"

using namespace ojgl;

ojstd::vector<Scene> Eldur::buildSceneGraph(const Vector2i& sceneSize) const
{
    ojstd::vector<Scene> scenes;
    {
        auto edison = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "lavaIntro.fs");
        auto fxaa = Buffer::construct(sceneSize.x, sceneSize.y, "fxaa.vs", "fxaa.fs");
        fxaa->setInputs(edison);

        auto post = Buffer::construct(sceneSize.x, sceneSize.y, "post.vs", "post.fs");
        post->setInputs(fxaa);

        scenes.emplace_back(post, Duration::seconds(20));
    }
    {
        auto noise = Buffer::construct(sceneSize.x, sceneSize.y, "demo.vs", "mountainNoise.fs");
        auto mountain = Buffer::construct(sceneSize.x, sceneSize.y, "demo.vs", "mountain.fs");
        mountain->setInputs(noise);

        auto iGlobalTimeCallback = [](float relativeSceneTime) {
            Buffer::UniformVector vector;
            vector.push_back(ojstd::make_shared<Uniform1f>("iGlobalTime", relativeSceneTime - 2.f));
            return vector;
        };
        mountain->setUniformCallback(iGlobalTimeCallback);

        auto fxaa = Buffer::construct(sceneSize.x, sceneSize.y, "fxaa.vs", "fxaa.fs");
        fxaa->setInputs(mountain);

        auto post = Buffer::construct(sceneSize.x, sceneSize.y, "demo.vs", "mountainPost.fs");
        post->setInputs(fxaa);
        post->setUniformCallback(iGlobalTimeCallback);

        scenes.emplace_back(post, Duration::seconds(77));
    }
    {
        auto edison = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "lavaScene2.fs");
        auto fxaa = Buffer::construct(sceneSize.x, sceneSize.y, "fxaa.vs", "fxaa.fs");
        fxaa->setInputs(edison);

        auto post = Buffer::construct(sceneSize.x, sceneSize.y, "post.vs", "post.fs");
        post->setInputs(fxaa);

        scenes.emplace_back(post, Duration::seconds(40));
    }
    {
        auto edison = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "outro.fs");
        auto fxaa = Buffer::construct(sceneSize.x, sceneSize.y, "fxaa.vs", "fxaa.fs");
        fxaa->setInputs(edison);

        auto post = Buffer::construct(sceneSize.x, sceneSize.y, "post.vs", "post.fs");
        post->setInputs(fxaa);

        scenes.emplace_back(post, Duration::seconds(40));
    }

    return scenes;
}

ojstd::string Eldur::getTitle() const
{
    return "OJ - Eldur";
}

static const unsigned char song[] = {
#include "songs/song.inc"
};

const unsigned char* Eldur::getSong() const
{
    return song;
}
