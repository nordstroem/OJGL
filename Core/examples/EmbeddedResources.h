#pragma once
#include <string>
namespace ojgl {

namespace resources {

    namespace fragment {

        std::string post{
#include "shaders/post.fs"
        };

        std::string fxaa{
#include "shaders/fxaa.fs"
        };

		std::string lavaIntro{
#include "done/lavaIntro.fs"
        };

		std::string mountain{
#include "done/mountain.fs"
        };

		std::string mountainNoise{
#include "done/mountainNoise.fs"
        };

		std::string mountainPost{
#include "done/mountainPost.fs"
        };
        std::string lavaScene2{
#include "done/lavaScene2.fs"
        };
        std::string outro{
#include "done/outro.fs"
        };        

        std::string demo{
#include "shaders/demo.fs"
        };

        std::string intro{
#include "shaders/introScene.fs"
        };
    } //namespace fragment

    namespace vertex {

        std::string demo{
#include "shaders/demo.vs"
        };

        std::string post{
#include "shaders/post.vs"
        };
        std::string edison{
#include "shaders/edison.vs"
        };
        std::string fxaa{
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
