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
    static BufferPtr construct(unsigned width, unsigned height, const std::string& name, const std::string& vertex, const std::string& fragment, std::initializer_list<BufferPtr> buffers);
    static BufferPtr construct(unsigned width, unsigned height, const std::string& name, const std::string& vertex, const std::string& fragment);

    unsigned getProgramID() const;
    unsigned fboTextureID();
    void render();
    std::string name();
    void generateFBO();

private:
    Buffer(unsigned width, unsigned height, const std::string& name, const std::string& vertex, const std::string& fragment, std::initializer_list<std::shared_ptr<Buffer>> buffers);
    void loadShader(const std::string& vertexShader, const std::string& fragmentShader);

    std::vector<BufferPtr> _inputs;
    const std::string _name;
    unsigned _programID;
    unsigned _fboID = 0;
    unsigned _fboTextureID = 0;
    const unsigned _width;
    const unsigned _height;
    std::unordered_map<std::string, std::shared_ptr<UniformBase>> _uniforms;
    std::unordered_map<std::string, std::shared_ptr<Uniform1t>> _textures;

    static constexpr unsigned vertexCount = 6;

public:
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
