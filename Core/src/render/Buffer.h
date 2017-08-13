#pragma once
#include "Uniform.hpp"
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace ojgl {

class Buffer {

    using BufferP = std::shared_ptr<Buffer>;

public:
    ~Buffer();
    static BufferP construct(unsigned width, unsigned height, const std::string& name, const std::string& vertex, const std::string& fragment, std::initializer_list<BufferP> buffers);
    static BufferP construct(unsigned width, unsigned height, const std::string& name, const std::string& vertex, const std::string& fragment);

    unsigned getProgramID() const;
    unsigned fboTextureID();
    void render();
    std::string name();
    void generateFBO();

private:
    Buffer(unsigned width, unsigned height, const std::string& name, const std::string& vertex, const std::string& fragment, std::initializer_list<std::shared_ptr<Buffer>> buffers);
    void loadShader(const std::string& vertexShader, const std::string& fragmentShader);

    std::vector<BufferP> _inputs;
    const std::string _name;
    unsigned _programID;
    unsigned _fboID = 0;
    unsigned _fboTextureID = 0;
    const unsigned _width;
    const unsigned _height;
    std::map<std::string, std::shared_ptr<UniformBase>> _uniforms;

public:
    template <typename T>
    friend Buffer& operator<<(Buffer& o, T& b);

    typedef std::vector<BufferP>::iterator iterator;
    typedef std::vector<BufferP>::const_iterator const_iterator;
    iterator begin() { return _inputs.begin(); }
    const_iterator begin() const { return _inputs.begin(); }
    iterator end() { return _inputs.end(); }
    const_iterator end() const { return _inputs.end(); }
};

template <typename T>
Buffer& operator<<(Buffer& o, T& b)
{
    o._uniforms[b.location()] = std::make_shared<typename std::remove_reference<T>::type>(std::forward<T>(b));
    return o;
}

} //namespace ojgl
