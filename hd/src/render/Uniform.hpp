#pragma once
#include "../../fakelibs/fakelib.h"
#include "../winapi/gl_loader.h"

namespace ojgl {

class UniformBase {
public:
    UniformBase(const fl::string& location)
        : _location(location)
    {
    }
    virtual ~UniformBase() = default;

    virtual void setUniform(int programID) = 0;
    fl::string location() const { return _location; }

protected:
    const fl::string _location;
};

class Uniform1f : public UniformBase {
public:
    Uniform1f(const fl::string& location, float x)
        : UniformBase(location)
        , _x(x){};
    void setUniform(int programID) override
    {
        glUniform1f(glGetUniformLocation(programID, _location.c_str()), this->_x);
    }

private:
    const float _x;
};

class Uniform2f : public UniformBase {
public:
    Uniform2f(const fl::string& location, float x, float y)
        : UniformBase(location)
        , _x(x)
        , _y(y)
    {
    }

    void setUniform(int programID) override
    {
        glUniform2f(glGetUniformLocation(programID, _location.c_str()), this->_x, this->_y);
    }

private:
    const float _x;
    const float _y;
};

class Uniform1fv : public UniformBase {
public:
    Uniform1fv(const fl::string& location, const fl::vector<float>& values)
        : UniformBase(location)
        , _values(values){};
    void setUniform(int programID) override
    {
        glUniform1fv(glGetUniformLocation(programID, this->_location.c_str()), _values.size(), &_values[0]);
    }

private:
    const fl::vector<float> _values;
};

class Uniform1t {
public:
    Uniform1t(const fl::string& location, const fl::shared_ptr<Texture>& texture)
        : _location(location)
        , _texture(texture){};
    fl::string location() const { return _location; }
    int textureID() const
    {
        /*return _texture->textureID();*/
        Texture* ptr = _texture.get();
        return ptr->textureID();
    }

private:
    const fl::string _location;
    const fl::shared_ptr<Texture> _texture;
};
} //namespace ojgl
