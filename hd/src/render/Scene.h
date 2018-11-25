#pragma once

#include "../utility/Timer.hpp"
#include "Buffer.h"
#include <memory>
#include <string>
#include <unordered_set>

namespace ojgl {

class Scene {
public:
    Scene(const std::shared_ptr<Buffer>& buffer /*, Duration duration*/);
    Scene(const std::string& name, const std::shared_ptr<Buffer>& buffer /*, Duration duration*/);

    void render();
    std::string name() const;
    Buffer& operator[](const std::string& name);
    /*Duration duration() const;*/
    std::unordered_set<std::shared_ptr<Buffer>> buffers();

private:
    std::shared_ptr<Buffer> _mainBuffer;
    /*Duration _duration;*/
    std::string _name;
};
} //namespace ojgl
