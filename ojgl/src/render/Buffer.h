#pragma once
#include "Mesh.h"
#include "Uniform.hpp"
#include "utility/OJstd.h"
#include "utility/Vector.h"

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

    Buffer& setFormat(BufferFormat format);
    Buffer& setDepthTest(bool depthTestEnabled);
    Buffer& setRenderOnce(bool renderOnce);
    Buffer& setNumOutTextures(int numOutTextures);
    Buffer& setName(const ojstd::string& name);
    template <typename... T>
    Buffer& setInputs(T... inputs)
    {
        _inputs = ojstd::vector<BufferPtr>({ inputs... });
        _numInputs = Buffer::getNumberOfInputs(_inputs);
        return *this;
    }

    ojstd::string name() const;
    void generateFBO();
    void render(const Vector2i& viewportOffset);
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

    static BufferPtr construct(unsigned width, unsigned height, const ojstd::string& vertexPath, const ojstd::string& fragmentPath);

private:
    Buffer(unsigned width, unsigned height, const ojstd::string& vertexPath, const ojstd::string& fragmentPath);
    void loadShader();
    int numOutTextures();
    static int getNumberOfInputs(const ojstd::vector<BufferPtr>& inputs);

private:
    ojstd::vector<BufferPtr> _inputs;
    int _numInputs = 0;
    ojstd::string _name = "default";
    const ojstd::string _vertexPath;
    const ojstd::string _fragmentPath;
    BufferFormat _format = BufferFormat::Quad;
    const unsigned _width;
    const unsigned _height;
    bool _renderOnce = false;
    bool _hasRendered = false;
    int _numOutTextures = 1;
    bool _depthTestEnabled = false;

    unsigned _programID = 0;
    unsigned _fboID = 0;
    unsigned _depthID = 0;
    ojstd::vector<unsigned> _fboTextureIDs;
    ojstd::unordered_map<ojstd::string, ojstd::shared_ptr<UniformBase>> _uniforms;
    ojstd::unordered_map<ojstd::string, ojstd::shared_ptr<Uniform1t>> _textures;
    ojstd::vector<ojstd::Pair<ojstd::shared_ptr<Mesh>, Matrix>> _meshes;
};

} //namespace ojgl
