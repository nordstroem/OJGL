#pragma once

namespace ojgl {
class Texture {
public:
    Texture(int, int, unsigned char*);
    ~Texture();

private:
    unsigned _width;
    unsigned _height;
    unsigned _textureID;
};
} //namespace ojgl