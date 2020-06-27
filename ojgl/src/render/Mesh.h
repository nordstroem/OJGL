#pragma once
#include "utility/OJstd.h"

namespace ojgl {
class Mesh {

public:
    Mesh(const Mesh& mesh) = delete;
    Mesh& operator=(const Mesh& mesh) = delete;
    ~Mesh();
    unsigned int vaoID() const { return _vaoID; }
    int verticesCount() const { return _verticesCount; };
    bool usesIndices() const { return _usesIndices; };

    static ojstd::shared_ptr<Mesh> construct(const ojstd::vector<float>& vertices);
    static ojstd::shared_ptr<Mesh> constructQuad();
    static ojstd::shared_ptr<Mesh> constructIndexedQuad();
    static ojstd::shared_ptr<Mesh> constructCube();
    static ojstd::shared_ptr<Mesh> constructIndexedCube();

private:
    Mesh(const ojstd::vector<float>& vertices, const ojstd::vector<float>& normals = {}, const ojstd::vector<float>& textureCoordinates = {}, const ojstd::vector<unsigned int>& indices = {});

    unsigned int _vaoID = 0;
    unsigned int _vboID = 0;
    unsigned int _nboID = 0;
    unsigned int _tboID = 0;
    unsigned int _iboID = 0;
    int _verticesCount = 0;
    bool _usesIndices = false;
};
}
