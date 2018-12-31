#pragma once

#include "utility/OJstd.h"
namespace ojgl {

namespace resources {

    namespace fragment {

        ojstd::string post {
#include "shaders/post.fs"
        };

        ojstd::string fxaa {
#include "shaders/fxaa.fs"
        };

        ojstd::string lavaIntro {
#include "shaders/lavaIntro.fs"
        };

        ojstd::string mountain {
#include "shaders/mountain.fs"
        };

        ojstd::string mountainNoise {
#include "shaders/mountainNoise.fs"
        };

        ojstd::string mountainPost {
#include "shaders/mountainPost.fs"
        };
        ojstd::string lavaScene2 {
#include "shaders/lavaScene2.fs"
        };
        ojstd::string outro {
#include "shaders/outro.fs"
        };

    } //namespace fragment

    namespace vertex {

        ojstd::string demo {
#include "shaders/demo.vs"
        };

        ojstd::string post {
#include "shaders/post.vs"
        };
        ojstd::string edison {
#include "shaders/edison.vs"
        };
        ojstd::string fxaa {
#include "shaders/fxaa.vs"
        };
    } //namespace vertex

    namespace songs {

        unsigned char song[] = {
#include "songs/song.inc"
        };
    } //namespace songs
}
}
