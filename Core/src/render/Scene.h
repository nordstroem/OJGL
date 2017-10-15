#pragma once

#include "Buffer.h"
#include "utility/Timer.hpp"
#include <memory>
#include <set>
#include <string>

namespace ojgl {

class Scene {
public:
    Scene(std::shared_ptr<Buffer> buffer, timer::ms_t);
    ~Scene();

    void render();
    Buffer& operator[](const std::string& name);

    std::set<std::shared_ptr<Buffer>> buffers();
    timer::ms_t duration() const;

private:
    std::shared_ptr<Buffer> _mainBuffer;
    timer::ms_t _duration;
};
} //namespace ojgl
