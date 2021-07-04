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
  #include "shaders/common/fxaa.fs"
          , "common/fxaa.fs"
          },

          {
  #include "shaders/common/fxaa.vs"
          , "common/fxaa.vs"
          },

          {
  #include "shaders/common/quad.vs"
          , "common/quad.vs"
          },

          {
#include "shaders/common/radial_blur.fs"
          , "common/radial_blur.fs"
          },
          {
  #include "shaders/common/fade.fs"
          , "common/fade.fs"
          },
          
          {
#include "shaders/edison2021/raymarch_settings.fs"
            , "edison2021/raymarch_settings.fs"
          },

          {
#include "shaders/edison2021/tunnel_base.fs"
            , "edison2021/tunnel_base.fs"
          },

          {
#include "shaders/edison2021/pipes.fs"
          , "edison2021/pipes.fs"
          },
          {
#include "shaders/edison2021/blob.fs"
          , "edison2021/blob.fs"
          },
          {
#include "shaders/edison2021/introText.fs"
          , "edison2021/introText.fs"
          },

          {
#include "shaders/edison2021/tunnel_transform.fs"
          , "edison2021/tunnel_transform.fs"
          },
          {
#include "shaders/edison2021/plane.fs"
          , "edison2021/plane.fs"
          },
          {
#include "shaders/edison2021/chrom_ab.fs"
          , "edison2021/chrom_ab.fs"
          },
  };
// clang-format on
}
