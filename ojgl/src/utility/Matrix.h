#pragma once

namespace ojgl {

class Matrix {
public:
    // data should be stored in column-major order.
    explicit Matrix(float data[16]);
    Matrix operator*(const Matrix& other);
    float* data() { return _data; }
    const float* data() const { return _data; }
    Matrix inverse() const;

    static Matrix perspective(float fovy, float aspect, float zNear, float zFar);
    static Matrix rotation(float x, float y, float z, float angle);
    static Matrix yRotation(float angle);
    static Matrix scaling(float scaling);
    static Matrix translation(float x, float y, float z);
    static Matrix identity();
    static Matrix fromQuaternion(float w, float x, float y, float z);
    static Matrix randomRotation();

private:
    float _data[16];
};
}
