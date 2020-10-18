#pragma once

#include "utility/OJstd.h"
namespace ojgl::resources {
// clang-format off
  ojstd::vector<ojstd::Pair<ojstd::string, ojstd::string>> shaders = {

          {
  #include "shaders/common/noise.fs"
          , "common/noise.fs"
          },

          {
  #include "shaders/common/primitives.fs"
          , "common/primitives.fs"
          },

          {
  #include "shaders/common/raymarch_settings.fs"
          , "common/raymarch_settings.fs"
          },

          {
  #include "shaders/common/raymarch_utils.fs"
          , "common/raymarch_utils.fs"
          },

          {
  #include "shaders/common/utils.fs"
          , "common/utils.fs"
          },

          {
  #include "shaders/common/raymarch_template.fs"
          , "common/raymarch_template.fs"
          },

          {
  #include "shaders/post.fs"
          , "post.fs"
          },

          {
  #include "shaders/fxaa.fs"
          , "fxaa.fs"
          },

          {
  #include "shaders/lavaIntro.fs"
          , "lavaIntro.fs"
          },

          {
  #include "shaders/mountain.fs"
          , "mountain.fs"
          },

          {
  #include "shaders/mountainNoise.fs"
          , "mountainNoise.fs"
          },

          {
  #include "shaders/mountainPost.fs"
          , "mountainPost.fs"
          },

          {
  #include "shaders/lavaScene2.fs"
          , "lavaScene2.fs"
          },

          {
  #include "shaders/outro.fs"
          , "outro.fs"
          },

          {
  #include "shaders/mesh.fs"
          , "mesh.fs"
          },

          {
  #include "shaders/cachedGeometry.fs"
          , "cachedGeometry.fs"
          },

          {
  #include "shaders/lightning.fs"
          , "lightning.fs"
          },

          {
  #include "shaders/demo.vs"
          , "demo.vs"
          },

          {
  #include "shaders/post.vs"
          , "post.vs"
          },

          {
  #include "shaders/edison.vs"
          , "edison.vs"
          },

          {
  #include "shaders/fxaa.vs"
          , "fxaa.vs"
          },

          {
  #include "shaders/mesh.vs"
          , "mesh.vs"
          },

          {
  #include "shaders/common/quad.vs"
          , "common/quad.vs"
          }
  };
// clang-format on

namespace songs {
    unsigned char song[] = {
#include "songs/song.inc"
    };
}
}
