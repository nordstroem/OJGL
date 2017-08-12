#pragma once
#include "winapi/gl_loader.h"
#include <memory>
#include <string>
#include <vector>

namespace ojgl {

class Buffer {

    using BufferP = std::shared_ptr<Buffer>;

public:
    ~Buffer();
    static BufferP construct(const std::string& name, const std::string& vertex, const std::string& fragment, std::initializer_list<BufferP> buffers);
    static BufferP construct(const std::string& name, const std::string& vertex, const std::string& fragment);
    GLuint getProgramID() const;

private:
    Buffer(const std::string& name, const std::string& vertex, const std::string& fragment, std::initializer_list<std::shared_ptr<Buffer>> buffers);
    void loadShader(const std::string& vertexShader, const std::string& fragmentShader);

    std::vector<BufferP> _inputs;
    std::string _name;
    GLuint _programID;
    int _fboID;
    bool _hasRun;
};
} //namespace ojgl
