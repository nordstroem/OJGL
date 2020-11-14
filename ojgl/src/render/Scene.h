#pragma once

#include "Buffer.h"
#include "utility/OJstd.h"
#include "utility/Timer.hpp"

namespace ojgl {

class Scene {
public:
    Scene(const ojstd::shared_ptr<Buffer>& buffer, Duration duration, const ojstd::string& name = "default");

    void render(float relativeSceneTime);
    ojstd::string name() const;
    Buffer& operator[](const ojstd::string& name);
    Duration duration() const;
    ojstd::unordered_set<ojstd::shared_ptr<Buffer>> buffers();
    ojstd::shared_ptr<Buffer> outputBuffer() const;

private:
    ojstd::shared_ptr<Buffer> _outputBuffer;
    Duration _duration;
    ojstd::string _name;
};
} //namespace ojgl
