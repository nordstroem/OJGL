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
  #include "shaders/eldur/post.fs"
          , "eldur/post.fs"
          },

          {
  #include "shaders/common/fxaa.fs"
          , "common/fxaa.fs"
          },

          {
  #include "shaders/eldur/lavaIntro.fs"
          , "eldur/lavaIntro.fs"
          },

          {
  #include "shaders/eldur/mountain.fs"
          , "eldur/mountain.fs"
          },

          {
  #include "shaders/eldur/mountainNoise.fs"
          , "eldur/mountainNoise.fs"
          },

          {
  #include "shaders/eldur/mountainPost.fs"
          , "eldur/mountainPost.fs"
          },

          {
  #include "shaders/eldur/lavaScene2.fs"
          , "eldur/lavaScene2.fs"
          },

          {
  #include "shaders/eldur/outro.fs"
          , "eldur/outro.fs"
          },

          {
  #include "shaders/experiments/cachedGeometry.fs"
          , "experiments/cachedGeometry.fs"
          },

          {
  #include "shaders/experiments/lightning.fs"
          , "experiments/lightning.fs"
          },

          {
  #include "shaders/common/fxaa.vs"
          , "common/fxaa.vs"
          },

          {
  #include "shaders/common/quad.vs"
          , "common/quad.vs"
          }
  };
// clang-format on

}
