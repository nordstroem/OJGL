#include "Texture.h"
#include "winapi\gl_loader.h"

namespace ojgl {

Texture::Texture(unsigned width, unsigned height, unsigned char* img)
    : _width(width)
    , _height(height)
{
    load(img);
}

Texture::~Texture()
{
    glDeleteTextures(1, &_textureID);
}

void Texture::load(unsigned char* img)
{
    glGenTextures(1, &_textureID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

} //namespace ojgl