#pragma once

#include "Buffer.h"
#include "utility/Timer.hpp"
#include <memory>
#include <string>
#include <unordered_set>

namespace ojgl {

class Scene {
public:
    Scene(std::shared_ptr<Buffer> buffer, timer::ms_t duration);
    ~Scene();

    void render();
    Buffer& operator[](const std::string& name);
    timer::ms_t duration() const;

private:
    std::unordered_set<std::shared_ptr<Buffer>> buffers();
    std::shared_ptr<Buffer> _mainBuffer;
    timer::ms_t _duration;
};
} //namespace ojgl
