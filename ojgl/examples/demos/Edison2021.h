#pragma once
#include "demo/Demo.h"
#include "render/Texture.h"

namespace ojgl {
class Edison2021 final : public Demo {
public:
    Edison2021();
    ojstd::vector<Scene> buildSceneGraph(const Vector2i& sceneSize) const override;
    ojstd::string getTitle() const override;
    const unsigned char* getSong() const override;

private:
    ojstd::shared_ptr<Texture> getText(const ojstd::string& text) const;
    mutable ojstd::unordered_map<ojstd::string, ojstd::shared_ptr<Texture>> _textures;
};
}