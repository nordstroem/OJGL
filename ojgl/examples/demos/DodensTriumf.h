#pragma once
#include "demo/Demo.h"

namespace ojgl {
  class DodensTriumf final : public Demo {
  public:
    ojstd::vector<Scene> buildSceneGraph(const Vector2i& sceneSize) const override;
    ojstd::string getTitle() const override;
    const unsigned char* getSong() const override;
  };
}