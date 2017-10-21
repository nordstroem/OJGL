#pragma once

namespace ojgl {
class Texture {
public:
    Texture(int width, int height, unsigned char* img);
    ~Texture();

private:
    void load(unsigned char* img);

    const int _width;
    const int _height;
    unsigned _textureID;
};
} //namespace ojgl