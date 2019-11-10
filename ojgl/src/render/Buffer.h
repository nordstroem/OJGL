#pragma once
#include "Mesh.h"
#include "Uniform.hpp"
#include "utility/OJstd.h"

namespace ojgl {

enum class BufferFormat {
    Quad,
    Meshes
};

class Buffer {

    using BufferPtr = ojstd::shared_ptr<Buffer>;

public:
    Buffer(const Buffer& other) = delete;
    ~Buffer();

    ojstd::string name() const;
    void generateFBO();
    void render();
    void insertMesh(const ojstd::shared_ptr<Mesh>& mesh, const Matrix& modelMatrix);
    void clearMeshes();

    auto begin() { return _inputs.begin(); }
    auto begin() const { return _inputs.cbegin(); }
    auto cbegin() const { return _inputs.cbegin(); }
    auto end() { return _inputs.end(); }
    auto end() const { return _inputs.cend(); }
    auto cend() const { return _inputs.cend(); }

    Buffer& operator<<(const Uniform1t& b);

    template <typename T>
    typename std::enable_if_t<std::is_base_of_v<UniformBase, typename std::remove_reference<T>::type>, Buffer&> operator<<(T&& b)
    {
        _uniforms[b.location()] = ojstd::make_shared<typename std::remove_reference<T>::type>(std::forward<T>(b));
        return *this;
    }

    static BufferPtr construct(unsigned width, unsigned height, const ojstd::string& name, const ojstd::string& vertexPath, const ojstd::string& fragmentPath, const ojstd::vector<BufferPtr>& inputs = {}, BufferFormat format = BufferFormat::Quad);

private:
    Buffer(unsigned width, unsigned height, const ojstd::string& name, const ojstd::string& vertexPath, const ojstd::string& fragmentPath, const ojstd::vector<BufferPtr>& inputs, BufferFormat format);
    void loadShader();

private:
    const ojstd::vector<BufferPtr> _inputs;
    const ojstd::string _name;
    const ojstd::string _vertexPath;
    const ojstd::string _fragmentPath;
    const BufferFormat _format;
    const unsigned _width;
    const unsigned _height;

    unsigned _programID = 0;
    unsigned _fboID = 0;
    unsigned _fboTextureID = 0;
    ojstd::unordered_map<ojstd::string, ojstd::shared_ptr<UniformBase>> _uniforms;
    ojstd::unordered_map<ojstd::string, ojstd::shared_ptr<Uniform1t>> _textures;
    ojstd::vector<ojstd::Pair<ojstd::shared_ptr<Mesh>, Matrix>> _meshes;
};

} //namespace ojgl
