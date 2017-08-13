#pragma once

#include "Buffer.h"
#include <memory>
#include <set>
#include <string>

namespace ojgl {

class Scene {
public:
    Scene(std::shared_ptr<Buffer> buffer);
    ~Scene();

    void render();
    Buffer& operator[](const std::string& name);

    std::set<std::shared_ptr<Buffer>> buffers();

private:
    std::shared_ptr<Buffer> _mainBuffer;
};
} //namespace ojgl
