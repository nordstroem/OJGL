#pragma once

#include "../utility/Timer.hpp"
#include "Buffer.h"
#include "utility/OJstd.h"

namespace ojgl {

class Scene {
public:
    Scene(const ojstd::shared_ptr<Buffer>& buffer, Duration duration);
    Scene(const ojstd::string& name, const ojstd::shared_ptr<Buffer>& buffer, Duration duration);

    void render();
    ojstd::string name() const;
    Buffer& operator[](const ojstd::string& name);
    Duration duration() const;
    ojstd::unordered_set<ojstd::shared_ptr<Buffer>> buffers();

private:
    ojstd::shared_ptr<Buffer> _mainBuffer;
    Duration _duration;
    ojstd::string _name;
};
} //namespace ojgl
