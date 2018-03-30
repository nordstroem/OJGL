#pragma once

#include "Buffer.h"
#include "utility/Timer.hpp"
#include <memory>
#include <string>
#include <unordered_set>

namespace ojgl {

class Scene {
public:
    Scene(const std::shared_ptr<Buffer>& buffer, Milliseconds duration);
    Scene(const std::string& name, const std::shared_ptr<Buffer>& buffer, Milliseconds duration);

    void render();
    std::string name() const;
    Buffer& operator[](const std::string& name);
    Milliseconds duration() const;

private:
    std::unordered_set<std::shared_ptr<Buffer>> buffers();
    std::shared_ptr<Buffer> _mainBuffer;
    Milliseconds _duration;
    std::string _name;
};
} //namespace ojgl
