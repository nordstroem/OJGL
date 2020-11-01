#pragma once
#include "demo/Demo.h"

namespace ojgl {
  class Template final : public Demo {
  public:
    Template() = default;
    ojstd::vector<Scene> buildSceneGraph(const Vector2i& sceneSize) const override;
    ojstd::string getTitle() const override;
  };
}
