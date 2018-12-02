#pragma once
#include "../fakelibs/fakelib.h"
namespace ojgl {

namespace resources {

    namespace fragment {

        fl::string test{
#include "shaders/test.fs"
        };

        fl::string test2{
#include "shaders/test2.fs"
        };

    } //namespace fragment

    namespace vertex {

        fl::string test{
#include "shaders/test.vs"
        };
    } //namespace vertex
}
}
