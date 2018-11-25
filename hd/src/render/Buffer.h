#pragma once
#include "Texture.h"
#include "Uniform.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace ojgl {

class Buffer {

    using BufferPtr = std::shared_ptr<Buffer>;

public:
    ~Buffer();
    unsigned getProgramID() const;
    unsigned fboTextureID();
    void render();
    std::string name() const;
    void generateFBO();

private:
    template <typename... Args>
    Buffer(unsigned width, unsigned height, const std::string& name, const std::string& vertexPath, const std::string& fragmentPath, Args&&... buffers)
        : _inputs({ std::forward<Args>(buffers)... })
        , _name(name)
        , _width(width)
        , _height(height)
        , _vertexPath(vertexPath)
        , _fragmentPath(fragmentPath)
    {
        loadShader();
    }

    void loadShader();

    std::vector<BufferPtr> _inputs;
    const std::string _name;
    unsigned _programID = 0;
    unsigned _fboID = 0;
    unsigned _fboTextureID = 0;
    const unsigned _width;
    const unsigned _height;
    std::unordered_map<std::string, std::shared_ptr<UniformBase>> _uniforms;
    std::unordered_map<std::string, std::shared_ptr<Uniform1t>> _textures;
    std::string _vertexPath;
    std::string _fragmentPath;

    static constexpr unsigned vertexCount = 6;

public:
    template <typename... Args>
    static BufferPtr construct(Args&&... args)
    {
        return std::shared_ptr<Buffer>(new Buffer(std::forward<Args>(args)...));
    }

    template <typename T>
    typename std::enable_if_t<std::is_base_of_v<UniformBase, typename std::remove_reference<T>::type>, Buffer&> operator<<(T&& b)
    {
        _uniforms[b.location()] = std::make_shared<typename std::remove_reference<T>::type>(std::forward<T>(b));
        return *this;
    }

    inline Buffer& operator<<(const Uniform1t& b)
    {
        _textures[b.location()] = std::make_shared<Uniform1t>(b);
        return *this;
    }

    auto begin() { return _inputs.begin(); }
    auto begin() const { return _inputs.cbegin(); }
    auto end() { return _inputs.end(); }
    auto end() const { return _inputs.cend(); }
};

} //namespace ojgl
