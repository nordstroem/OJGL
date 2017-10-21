#include "Texture.h"
#include "winapi\gl_loader.h"
#include <string>

namespace ojgl {
Texture::Texture(const std::string& location, int width, int height, int channels, unsigned char* img)
    : _location(location)
    , _width(width)
    , _height(height)
    , _channels(channels)
{
    load(img);
}

Texture::~Texture()
{
    glDeleteTextures(1, &_textureID);
}

std::string Texture::location() const
{
    return _location;
}

unsigned Texture::textureID()
{
    return _textureID;
}

void Texture::load(unsigned char* img)
{
    glGenTextures(1, &_textureID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, _channels == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, img);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

} //namespace ojgl