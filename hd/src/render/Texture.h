#pragma once
//#include <memory>
#include "../../fakelibs/fakelib.h"

namespace ojgl {
class Texture {
public:
    static fl::shared_ptr<Texture> construct(int width, int height, int channels, unsigned char* img);
    ~Texture();
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