#pragma once
#include "utility/OJstd.h"

namespace ojgl {
class Texture {
public:
    static ojstd::shared_ptr<Texture> construct(int width, int height, int channels, unsigned char* img);
    ~Texture();
    Texture& operator=(Texture other) = delete;

    unsigned textureID();

private:
    void load(unsigned char* img);
    Texture(int width, int height, int channels, unsigned char* img);

    const int _width;
    const int _height;
    const int _channels;
    unsigned _textureID;
};
} //namespace ojgl
