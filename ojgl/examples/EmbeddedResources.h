#pragma once

#include "utility/OJstd.h"
namespace ojgl {

namespace resources {

    namespace fragment {

        ojstd::string mesh {
#include "shaders/geometry-with-physics/mesh.fs"
        };
        ojstd::string rayMarch {
#include "shaders/geometry-with-physics/rayMarch.fs"
        };
        ojstd::string smoke {
#include "shaders/geometry-with-physics/smoke.fs"
        };
        ojstd::string sphere {
#include "shaders/geometry-with-physics/sphere.fs"
        };
        ojstd::string noise {
#include "shaders/common/noise.fs"
        };
        ojstd::string primitives {
#include "shaders/common/primitives.fs"
        };
        ojstd::string raymarch_utils {
#include "shaders/common/raymarch_utils.fs"
        };
        ojstd::string utils {
#include "shaders/common/utils.fs"
        };

    } //namespace fragment

    namespace vertex {
        ojstd::string mesh {
#include "shaders/geometry-with-physics/mesh.vs"
        };
        ojstd::string rayMarch {
#include "shaders/geometry-with-physics/rayMarch.vs"
        };
        ojstd::string smoke {
#include "shaders/geometry-with-physics/smoke.vs"
        };
        ojstd::string sphere {
#include "shaders/geometry-with-physics/sphere.vs"
        };
    } //namespace vertex

    namespace songs {

        unsigned char song[] = {
#include "songs/song.inc"
        };
    } //namespace songs
}
}
