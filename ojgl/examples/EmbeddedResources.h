#pragma once

#include "utility/OJstd.h"
namespace ojgl::resources {
// clang-format off
  ojstd::vector<ojstd::Pair<ojstd::string, ojstd::string>> shaders = {
          {
  #include "shaders/common/passThrough.fs"
          , "common/passThrough.fs"
          },
          {
  #include "shaders/common/quad.vs"
          , "common/quad.vs"
          },
          {
#include "shaders/edison2026/cube.fs"
          , "edison2026/cube.fs"
          }
  };
// clang-format on
}
