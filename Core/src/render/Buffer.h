#pragma once
#include "Texture.h"
#include "Uniform.hpp"
#include <iostream>
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
    std::map<std::string, std::shared_ptr<Uniform1t>> _textures;

public:
    template <typename T>
    typename std::enable_if_t<std::is_base_of_v<UniformBase, T>, Buffer&> operator<<(T& b)
    {
        _uniforms[b.location()] = std::make_shared<typename std::remove_reference<T>::type>(std::forward<T>(b));
        return *this;
    }

    template <typename T>
    typename std::enable_if_t<std::is_same_v<Uniform1t, T>, Buffer&> operator<<(T b)
    {
        _textures[b.location()] = std::make_shared<typename std::remove_reference<T>::type>(std::forward<T>(b));
        return *this;
    }

    typedef std::vector<BufferP>::iterator iterator;
    typedef std::vector<BufferP>::const_iterator const_iterator;
    iterator begin() { return _inputs.begin(); }
    const_iterator begin() const { return _inputs.begin(); }
    iterator end() { return _inputs.end(); }
    const_iterator end() const { return _inputs.end(); }
};

} //namespace ojgl
