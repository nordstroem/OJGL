#pragma once

namespace ojgl {
class Texture {
public:
    Texture(unsigned width, unsigned height, unsigned char* img);
    ~Texture();

private:
    void load(unsigned char* img);

    const unsigned _width;
    const unsigned _height;
    unsigned _textureID;
};
} //namespace ojgl