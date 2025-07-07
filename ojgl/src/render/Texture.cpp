#include "Texture.h"
#include "winapi/gl_loader.h"

namespace ojgl {
Texture::Texture(int width, int height, int channels, unsigned char* img)
    : _width(width)
    , _height(height)
    , _channels(channels)
{
    load(img);
}

ojstd::shared_ptr<Texture> Texture::construct(int width, int height, int channels, unsigned char* img)
{
    return ojstd::shared_ptr<Texture>(new Texture(width, height, channels, img));
}

Texture::~Texture()
{
    glDeleteTextures(1, &_textureID);
}

unsigned Texture::textureID()
{
    return _textureID;
}

void Texture::load(unsigned char* img)
{
    GLenum glType = [this]() {
      switch (_channels) {
      case 1: return GL_LUMINANCE;
      case 3: return GL_RGB;
      case 4: return GL_RGBA;
      default:
        _ASSERTE(false);
        return GL_RGBA;
      } }();

    glGenTextures(1, &_textureID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, glType, GL_UNSIGNED_BYTE, img);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace ojgl
