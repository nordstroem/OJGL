#pragma once
#include "utility/OJstd.h"

namespace ojgl {

namespace resources {

    namespace fragment {

        ojstd::string test {
#include "shaders/test.fs"
        };

        ojstd::string test2 {
#include "shaders/test2.fs"
        };

    } //namespace fragment

    namespace vertex {

        ojstd::string test {
#include "shaders/test.vs"
        };
    } //namespace vertex

    namespace songs {

        unsigned char song[] = {
#include "songs/song.inc"
        };
    } //namespace songs
}
}
