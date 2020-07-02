#pragma once

#include "utility/OJstd.h"
namespace ojgl {

namespace resources {

    namespace fragment {

        namespace fibberReborn {
            ojstd::string tunnel {
#include "shaders/fibber-reborn/tunnel.fs"

            };
            ojstd::string tower {
#include "shaders/fibber-reborn/tower.fs"

            };
            ojstd::string raymarchUtils {
#include "shaders/fibber-reborn/raymarch_utils.fs"
            };

        }

        ojstd::string mesh {
#include "shaders/geometry-with-physics/mesh.fs"
        };
        ojstd::string rayMarch {
#include "shaders/geometry-with-physics/rayMarch.fs"
        };
        ojstd::string sphere {
#include "shaders/geometry-with-physics/sphere.fs"
        };
        ojstd::string blur1 {
#include "shaders/geometry-with-physics/blur1.fs"
        };
        ojstd::string blur2 {
#include "shaders/geometry-with-physics/blur2.fs"
        };
        ojstd::string post {
#include "shaders/geometry-with-physics/post.fs"
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
        ojstd::string sphere {
#include "shaders/geometry-with-physics/sphere.vs"
        };
        ojstd::string quad {
#include "shaders/common/quad.vs"
        };
    } //namespace vertex

    namespace songs {

        unsigned char song[] = {
#include "songs/song.inc"
        };
    } //namespace songs
}
}
