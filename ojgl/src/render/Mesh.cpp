#include "Mesh.h"
#include "utility/OJstd.h"
#include "winapi/gl_loader.h"

using namespace ojgl;

static ojstd::Pair<unsigned int, unsigned int> generateVAO(const ojstd::vector<float>& vertices)
{
    unsigned int vaoID, vboID;

    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);
    glEnableVertexAttribArray(0);
    glGenBuffers(1, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.begin(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return { vaoID, vboID };
}

Mesh::Mesh(ojstd::vector<float> vertices)
{
    auto [vaoID, vboID] = generateVAO(vertices);
    _vaoID = vaoID;
    _vboID = vboID;
    _verticesCount = vertices.size();
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &_vaoID);
    glDeleteBuffers(1, &_vboID);
}

ojstd::shared_ptr<Mesh> Mesh::construct(const ojstd::vector<float>& vertices)
{
    return new Mesh(vertices);
}

ojstd::shared_ptr<Mesh> Mesh::constructQuad()
{
    ojstd::vector<float> vertices = {
        -1, 1, 0, -1, -1, 0, 1, -1, 0,
        1, -1, 0, 1, 1, 0, -1, 1, 0
    };
    return new Mesh(vertices);
}
