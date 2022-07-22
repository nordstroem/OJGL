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
#include "shaders/edison2022/raymarch_settings.fs"
          , "edison2022/raymarch_settings.fs"
          },
         {
 #include "shaders/edison2022/raymarch_settings_refract.fs"
          , "edison2022/raymarch_settings_refract.fs"
          },
          {
#include "shaders/edison2022/chrom_ab.fs"
          , "edison2022/chrom_ab.fs"
          },
          {
#include "shaders/edison2022/grass.fs"
          , "edison2022/grass.fs"
          },
          {
#include "shaders/edison2022/jellyfish.fs"
          , "edison2022/jellyfish.fs"
          },
         {
#include "shaders/edison2022/intro.fs"
          , "edison2022/intro.fs"
          },
          {
#include "shaders/edison2022/ascent.fs"
          , "edison2022/ascent.fs"
          },
          {
#include "shaders/edison2022/ship.fs"
          , "edison2022/ship.fs"
          },
          {
#include "shaders/edison2022/ship_chrom_ab.fs"
          , "edison2022/ship_chrom_ab.fs"
          },
          {
#include "shaders/edison2022/blur1.fs"
          , "edison2022/blur1.fs"
          },
  };
// clang-format on
}
