#pragma once
#include "../../fakelibs/fakelib.h"
#include "Texture.h"
#include "Uniform.hpp"
#include <type_traits>
#include <utility>

namespace ojgl {

class Buffer {

    using BufferPtr = fl::shared_ptr<Buffer>;

public:
    ~Buffer();
    unsigned getProgramID() const;
    unsigned fboTextureID();
    void render();
    fl::string name() const;
    void generateFBO();

private:
    template <typename... Args>
    Buffer(unsigned width, unsigned height, const fl::string& name, const fl::string& vertexPath, const fl::string& fragmentPath, Args&&... buffers)
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

    fl::vector<BufferPtr> _inputs;
    const fl::string _name;
    unsigned _programID = 0;
    unsigned _fboID = 0;
    unsigned _fboTextureID = 0;
    const unsigned _width;
    const unsigned _height;
    fl::unordered_map<fl::string, fl::shared_ptr<UniformBase>> _uniforms;
    fl::unordered_map<fl::string, fl::shared_ptr<Uniform1t>> _textures;
    fl::string _vertexPath;
    fl::string _fragmentPath;

    static constexpr unsigned vertexCount = 6;

public:
    template <typename... Args>
    static BufferPtr construct(Args&&... args)
    {
        return fl::shared_ptr<Buffer>(new Buffer(std::forward<Args>(args)...));
    }

    template <typename T>
    typename std::enable_if_t<std::is_base_of_v<UniformBase, typename std::remove_reference<T>::type>, Buffer&> operator<<(T&& b)
    {
        _uniforms[b.location()] = fl::make_shared<typename std::remove_reference<T>::type>(std::forward<T>(b));
        return *this;
    }

    inline Buffer& operator<<(const Uniform1t& b)
    {
        _textures[b.location()] = fl::make_shared<Uniform1t>(b);
        return *this;
    }

    auto begin() { return _inputs.begin(); }
    auto begin() const { return _inputs.cbegin(); }
    auto end() { return _inputs.end(); }
    auto end() const { return _inputs.cend(); }
};

} //namespace ojgl
