#pragma once
#include "demo/Demo.h"

namespace ojgl {
  class QED final : public Demo {
  public:
    QED() = default;
    ojstd::vector<Scene> buildSceneGraph(const Vector2i& sceneSize) const override;
    ojstd::string getTitle() const override;
  };
}
