#pragma once

#include "Buffer.h"
#include <memory>
#include <string>

namespace ojgl {

class Scene {
public:
    Scene(std::shared_ptr<Buffer> buffer);
    ~Scene();
    void render();
    Buffer& operator[](const std::string& name);

private:
    std::shared_ptr<Buffer> _mainBuffer;
};
} //namespace ojgl
