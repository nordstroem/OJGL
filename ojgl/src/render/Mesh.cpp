#include "Mesh.h"
#include "utility/OJstd.h"
#include "winapi/gl_loader.h"

using namespace ojgl;

struct BufferIndices {
    unsigned int vaoID = 0;
    unsigned int vboID = 0;
    unsigned int nboID = 0;
    unsigned int tboID = 0;
    unsigned int iboID = 0;
};

static BufferIndices generateVAO(const ojstd::vector<float>& vertices, const ojstd::vector<float>& normals = {}, const ojstd::vector<float>& textureCoordinates = {}, const ojstd::vector<unsigned int>& indices = {})
{
    unsigned int vaoID = 0;
    unsigned int vboID = 0;
    unsigned int nboID = 0;
    unsigned int tboID = 0;
    unsigned int iboID = 0;

    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    if (!vertices.empty()) {
        glEnableVertexAttribArray(0);
        glGenBuffers(1, &vboID);
        glBindBuffer(GL_ARRAY_BUFFER, vboID);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.begin(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    }

    if (!normals.empty()) {
        glGenBuffers(1, &nboID);
        glBindBuffer(GL_ARRAY_BUFFER, nboID);
        glEnableVertexAttribArray(1);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.begin(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, nullptr);
    }

    if (!textureCoordinates.empty()) {
        glGenBuffers(1, &tboID);
        glBindBuffer(GL_ARRAY_BUFFER, tboID);
        glEnableVertexAttribArray(2);
        glBufferData(GL_ARRAY_BUFFER, textureCoordinates.size() * sizeof(float), textureCoordinates.begin(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, false, 0, nullptr);
    }

    if (!indices.empty()) {
        glGenBuffers(1, &iboID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return { vaoID, vboID, nboID, tboID, iboID };
}

Mesh::Mesh(const ojstd::vector<float>& vertices, const ojstd::vector<float>& normals, const ojstd::vector<float>& textureCoordinates, const ojstd::vector<unsigned int>& indices)
{
    auto [vaoID, vboID, nboID, tboID, iboID] = generateVAO(vertices, normals, textureCoordinates, indices);
    _vaoID = vaoID;
    _vboID = vboID;
    _nboID = nboID;
    _tboID = tboID;
    _iboID = iboID;
    _verticesCount = indices.size() > 0 ? indices.size() : vertices.size();
    _usesIndices = indices.size() > 0;
}

Mesh::~Mesh()
{
    if (_vaoID)
        glDeleteVertexArrays(1, &_vaoID);
    if (_vboID)
        glDeleteBuffers(1, &_vboID);
    if (_nboID)
        glDeleteBuffers(1, &_nboID);
    if (_tboID)
        glDeleteBuffers(1, &_tboID);
    if (_iboID)
        glDeleteBuffers(1, &_iboID);
}

ojstd::shared_ptr<Mesh> Mesh::construct(const ojstd::vector<float>& vertices)
{
    return new Mesh(vertices);
}

ojstd::shared_ptr<Mesh> Mesh::constructQuad()
{
    // clang-format off
    ojstd::vector<float> vertices = {
        -1, 1, 0,
        -1, -1, 0,
        1, -1, 0,
        1, -1, 0,
        1, 1, 0,
        -1, 1, 0
    };
    ojstd::vector<float> normals = {
        0, 0, 1,
        0, 0, 1,
        0, 0, 1,
        0, 0, 1,
        0, 0, 1,
        0, 0, 1,
    };
	ojstd::vector<float> textureCoordinates = {
        0, 1,
        0, 0,
        1, 0,
        1, 0,
        1, 1,
        0, 1, 
    };
    // clang-format on
    return new Mesh(vertices, normals, textureCoordinates);
}

ojstd::shared_ptr<Mesh> Mesh::constructIndexedQuad()
{
    // clang-format off
    ojstd::vector<float> vertices = {
        -1, 1, 0,
        -1, -1, 0,
        1, -1, 0,
        1, 1, 0
    };

    ojstd::vector<float> normals = {
        0, 0, 1,
        0, 0, 1,
        0, 0, 1,
        0, 0, 1,
    };
	ojstd::vector<float> textureCoordinates = {
        0, 1,
        0, 0,
        1, 0,
        1, 1,
    };

    ojstd::vector<unsigned int> indices = {0, 1, 2, 2, 3, 0};
    // clang-format on
    return new Mesh(vertices, normals, textureCoordinates, indices);
}

ojstd::shared_ptr<Mesh> Mesh::constructIndexedCube()
{
    // clang-format off
    ojstd::vector<float> vertices = {
        -1, 1, -1,
        1, -1, -1,
        1, 1, -1,
        -1, 1, -1,
        -1, -1, 1,
        1, -1, 1,
        1, 1, 1,
        -1, 1, 1
    };

    ojstd::vector<float> normals = {
        0, 0, 1,
        1, 0, 0,
        0, 0, -1,
        -1, 0, 0,
        0, 1, 0,
        0, -1, 0,
    };
	ojstd::vector<float> textureCoordinates = {
        0, 0,
        1, 0,
        1, 1,
        0, 1,
    };

    ojstd::vector<unsigned int> indices = {0, 1, 2, 2, 3, 0};
    // clang-format on
    return new Mesh(vertices, normals, textureCoordinates, indices);
}

ojstd::shared_ptr<Mesh> Mesh::constructCube()
{
    // clang-format off
    ojstd::vector<float> vertices = {
        // Front face
        -1.0, -1.0, 1.0,
         1.0, -1.0, 1.0,
         1.0,  1.0, 1.0,

        -1.0,  1.0, 1.0,
         1.0,  1.0, 1.0,
        -1.0, -1.0, 1.0,

        // Back face
         1.0, -1.0, -1.0,
        -1.0, -1.0, -1.0,
         1.0,  1.0, -1.0,

        -1.0,  1.0, -1.0,
         1.0,  1.0, -1.0,
        -1.0, -1.0, -1.0,

        // Top face
        -1.0, 1.0, -1.0, 
         1.0, 1.0, -1.0, 
         1.0, 1.0,  1.0, 

        -1.0, 1.0,  1.0, 
         1.0, 1.0,  1.0, 
        -1.0, 1.0, -1.0, 

        // Bottom face
        -1.0, -1.0, -1.0, 
         1.0, -1.0, -1.0, 
         1.0, -1.0,  1.0, 

        -1.0, -1.0,  1.0, 
         1.0, -1.0,  1.0, 
        -1.0, -1.0, -1.0,

        // Right face
        1.0, -1.0, -1.0, 
        1.0,  1.0, -1.0, 
        1.0,  1.0,  1.0, 

        1.0, -1.0,  1.0, 
        1.0,  1.0,  1.0, 
        1.0, -1.0, -1.0, 

        // Left face
        -1.0, -1.0, -1.0, 
        -1.0,  1.0, -1.0, 
        -1.0,  1.0,  1.0, 
        
        -1.0, -1.0,  1.0, 
        -1.0,  1.0,  1.0, 
        -1.0, -1.0, -1.0, 
    };
    // clang-format on

    ojstd::vector<float> normal = {
        // Front face
        0.0, 0.0, 1.0,
        0.0, 0.0, 1.0,
        0.0, 0.0, 1.0,
        0.0, 0.0, 1.0,
        0.0, 0.0, 1.0,
        0.0, 0.0, 1.0,

        // Back face
        0.0, 0.0, -1.0,
        0.0, 0.0, -1.0,
        0.0, 0.0, -1.0,
        0.0, 0.0, -1.0,
        0.0, 0.0, -1.0,
        0.0, 0.0, -1.0,

        // Top face
        0.0, 1.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 1.0, 0.0,

        // Bottom face
        0.0, -1.0, 0.0,
        0.0, -1.0, 0.0,
        0.0, -1.0, 0.0,
        0.0, -1.0, 0.0,
        0.0, -1.0, 0.0,
        0.0, -1.0, 0.0,

        // Right face
        1.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        1.0, 0.0, 0.0,

        // Left face
        -1.0, 0.0, 0.0,
        -1.0, 0.0, 0.0,
        -1.0, 0.0, 0.0,
        -1.0, 0.0, 0.0,
        -1.0, 0.0, 0.0,
        -1.0, 0.0, 0.0
    };
    return new Mesh(vertices, normal);
}
