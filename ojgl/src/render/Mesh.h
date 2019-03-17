#pragma once
#include "utility/OJstd.h"

namespace ojgl {
class Mesh {

public:
    ~Mesh();
    unsigned int vaoID() const { return _vaoID; }
    int verticesCount() const { return _verticesCount; };

    static ojstd::shared_ptr<Mesh> construct(const ojstd::vector<float>& vertices);
    static ojstd::shared_ptr<Mesh> constructQuad();

private:
    Mesh(ojstd::vector<float> vertices);

    unsigned int _vaoID;
    unsigned int _vboID;
    int _verticesCount;
};
}
