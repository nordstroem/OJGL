#include "Matrix.h"
#include "utility/Log.h"
#include "utility/OJstd.h"
#include <string.h>

using namespace ojgl;

Matrix::Matrix(float data[16])
{
    memcpy(_data, data, sizeof(float) * 16);
}

Matrix Matrix::perspective(float fovy, float aspect, float zNear, float zFar)
{
    float tf = ojstd::tan(fovy / 2);
    float m1 = 1 / (aspect * tf);
    float m6 = 1 / tf;
    float m11 = -(zFar + zNear) / (zFar - zNear);
    float m12 = -1;
    float m15 = -2 * zFar * zNear / (zFar - zNear);
    float data[16] = { m1, 0, 0, 0, 0, m6, 0, 0, 0, 0, m11, m12, 0, 0, m15, 0 };
    return Matrix(data);
}

// https://www.gamedev.net/forums/topic/671079-fast-sqrt-for-64bit/
double inline __declspec(naked) __fastcall sqrt_asm(double n)
{
    _asm {
		fld qword ptr[esp + 4]
		fsqrt
		ret 8
    }
}

Matrix Matrix::rotation(float x, float y, float z, float angle)
{
    float n = sqrt_asm(x * x + y * y + z * z);
    _ASSERTE(n != 0);
    x /= n;
    y /= n;
    z /= n;

    float c = ojstd::cos(angle);
    float s = ojstd::sin(angle);
    float m1 = x * x * (1 - c) + c;
    float m2 = y * x * (1 - c) + z * s;
    float m3 = z * x * (1 - c) - y * s;
    float m5 = x * y * (1 - c) - z * s;
    float m6 = y * y * (1 - c) + c;
    float m7 = z * y * (1 - c) + x * s;
    float m9 = x * z * (1 - c) + y * s;
    float m10 = y * z * (1 - c) - x * s;
    float m11 = z * z * (1 - c) + c;
    float data[16] = { m1, m2, m3, 0, m5, m6, m7, 0, m9, m10, m11, 0, 0, 0, 0, 1 };
    return Matrix(data);
}

Matrix Matrix::identity()
{
    float data[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
    return Matrix(data);
}

Matrix Matrix::scaling(float scaling)
{
    float data[16] = { scaling, 0, 0, 0, 0, scaling, 0, 0, 0, 0, scaling, 0, 0, 0, 0, 1 };
    return Matrix(data);
}

Matrix Matrix::translation(float x, float y, float z)
{
    float data[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, x, y, z, 1 };
    return Matrix(data);
}

Matrix Matrix::operator*(const Matrix& other)
{
    float result[16];
    auto A = [this](int row, int column) -> const float& { return this->_data[column * 4 + row]; };
    auto B = [&other](int row, int column) -> const float& { return other.data()[column * 4 + row]; };
    auto C = [&result](int row, int column) -> float& { return result[column * 4 + row]; };

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            float sum = 0.f;
            for (int k = 0; k < 4; k++) {
                sum += A(i, k) * B(k, j);
            }
            C(i, j) = sum;
        }
    }

    return Matrix(result);
}

Matrix Matrix::inverse() const
{
    float inv[16];
    float det = 0.0;
    const float* m = &this->_data[0];

    inv[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] + m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];
    inv[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] - m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];
    inv[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] + m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];
    inv[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] - m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];
    inv[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] - m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];
    inv[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] + m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];
    inv[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] - m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[9];
    inv[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] + m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[9];
    inv[2] = m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15] + m[5] * m[3] * m[14] + m[13] * m[2] * m[7] - m[13] * m[3] * m[6];
    inv[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15] - m[4] * m[3] * m[14] - m[12] * m[2] * m[7] + m[12] * m[3] * m[6];
    inv[10] = m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15] + m[4] * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];
    inv[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14] - m[4] * m[2] * m[13] - m[12] * m[1] * m[6] + m[12] * m[2] * m[5];
    inv[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] - m[5] * m[3] * m[10] - m[9] * m[2] * m[7] + m[9] * m[3] * m[6];
    inv[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11] + m[4] * m[3] * m[10] + m[8] * m[2] * m[7] - m[8] * m[3] * m[6];
    inv[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11] - m[4] * m[3] * m[9] - m[8] * m[1] * m[7] + m[8] * m[3] * m[5];
    inv[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10] + m[4] * m[2] * m[9] + m[8] * m[1] * m[6] - m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    _ASSERTE(det != 0);

    det = 1.f / det;

    for (int i = 0; i < 16; i++)
        inv[i] = inv[i] * det;

    return Matrix(inv);
}
