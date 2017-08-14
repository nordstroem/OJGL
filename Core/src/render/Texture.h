#pragma once

namespace ojgl {
class Texture {
public:
    Texture();
    ~Texture();
    void load(unsigned char*);

private:
    unsigned _width;
    unsigned _height;
    unsigned _textureID;
};
} //namespace ojgl