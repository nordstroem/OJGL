#pragma once

#include "../../fakelibs/fakelib.h"
#include "../utility/Timer.hpp"
#include "Buffer.h"

namespace ojgl {

class Scene {
public:
    Scene(const fl::shared_ptr<Buffer>& buffer /*, Duration duration*/);
    Scene(const fl::string& name, const fl::shared_ptr<Buffer>& buffer /*, Duration duration*/);

    void render();
    fl::string name() const;
    Buffer& operator[](const fl::string& name);
    /*Duration duration() const;*/
    fl::vector<fl::shared_ptr<Buffer>> buffers();

private:
    fl::shared_ptr<Buffer> _mainBuffer;
    /*Duration _duration;*/
    fl::string _name;
};
} //namespace ojgl
