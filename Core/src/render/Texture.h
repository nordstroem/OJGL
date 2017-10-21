#pragma once
#include <string>

namespace ojgl {
class Texture {
public:
    Texture(const std::string& location, int width, int height, int channels, unsigned char* img);
    ~Texture();
    std::string location() const;
    unsigned textureID();

private:
    void load(unsigned char* img);

    const std::string _location;
    const int _width;
    const int _height;
    const int _channels;
    unsigned _textureID;
};
} //namespace ojgl