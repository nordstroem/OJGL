#pragma once
#include "Uniform.hpp"
#include "winapi/gl_loader.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace ojgl {

class Buffer {

    using BufferP = std::shared_ptr<Buffer>;

public:
    ~Buffer();
    static BufferP construct(const std::string& name, const std::string& vertex, const std::string& fragment, std::initializer_list<BufferP> buffers);
    static BufferP construct(const std::string& name, const std::string& vertex, const std::string& fragment);
    GLuint getProgramID() const;
    void render();
    void reset();
    bool hasRun();

    template <typename T>
    friend Buffer& operator<<(Buffer& o, T& b);

private:
    Buffer(const std::string& name, const std::string& vertex, const std::string& fragment, std::initializer_list<std::shared_ptr<Buffer>> buffers);
    void loadShader(const std::string& vertexShader, const std::string& fragmentShader);

    std::vector<BufferP> _inputs;
    std::string _name;
    GLuint _programID;
    int _fboID;
    bool _hasRun;
    std::map<std::string, std::shared_ptr<UniformBase>> _uniforms;
};

template <typename T>
Buffer& operator<<(Buffer& o, T& b)
{
    o._uniforms[b.location()] = std::make_shared<typename std::remove_reference<T>::type>(std::forward<T>(b));
    return o;
}

} //namespace ojgl
