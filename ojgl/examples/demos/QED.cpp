#include "QED.h"

using namespace ojgl;

ojstd::vector<Scene> QED::buildSceneGraph(const Vector2i& sceneSize) const
{
    ojstd::vector<Scene> scenes;
    {
        auto tower = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "QED/tower.fs");

        auto blur1 = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "QED/blur1.fs");
        blur1->setInputs(tower);
        blur1->setUniformCallback([](float relativeSceneTime) -> Buffer::UniformVector {
          return { ojstd::make_shared<Uniform2f>("blurDir", 1, 0) };
        });

        auto blur2 = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "QED/blur1.fs");
        blur2->setInputs(blur1);
        blur2->setUniformCallback([](float relativeSceneTime) -> Buffer::UniformVector {
            return { ojstd::make_shared<Uniform2f>("blurDir", 0, 1) };
        });

        auto towerPost = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "QED/post.fs");
        towerPost->setInputs(blur2);

        scenes.emplace_back(towerPost, Duration::seconds(8 + 15 + 17 + 12));
    }

    {
        auto sphere = Buffer::construct(sceneSize.x, sceneSize.y, "QED/sphere.vs", "QED/sphere.fs");
        sphere->setFormat(BufferFormat::Meshes);
        sphere->setNumOutTextures(2);
        sphere->setDepthTest(true);

        auto rayMarch = Buffer::construct(sceneSize.x, sceneSize.y, "QED/rayMarch.vs", "QED/rayMarch.fs");
        rayMarch->setInputs(sphere);

        auto blur1 = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "QED/blur1.fs");
        blur1->setInputs(rayMarch);
        blur1->setUniformCallback([](float relativeSceneTime) -> Buffer::UniformVector {
          return { ojstd::make_shared<Uniform2f>("blurDir", 1, 0) };
        });


        auto blur2 = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "QED/blur1.fs");
        blur2->setInputs(blur1);
        blur2->setUniformCallback([](float relativeSceneTime) -> Buffer::UniformVector {
            return { ojstd::make_shared<Uniform2f>("blurDir", 0, 1) };
        });

        auto post = Buffer::construct(sceneSize.x, sceneSize.y, "common/quad.vs", "QED/post.fs");
        post->setInputs(blur2);

        scenes.emplace_back(post, Duration::seconds(63));
    }

    return scenes;
}

ojstd::string QED::getTitle() const
{
    return "OJ - Q.E.D.";
}
