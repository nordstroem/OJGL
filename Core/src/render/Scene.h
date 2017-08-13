#pragma once

#include "Buffer.h"
#include <memory>

namespace ojgl {

class Scene {
public:
    Scene(std::shared_ptr<Buffer> buffer);
    ~Scene();
    void render();
    std::shared_ptr<Buffer> _mainBuffer;

    //private:
    //    std::shared_ptr<Buffer> _mainBuffer;
};
} //namespace ojgl
