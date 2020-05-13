#pragma once

template <typename T>
struct Vector3 {
    T x = T(0);
    T y = T(0);
    T z = T(0);

    Vector3 Vector3::operator*(T a)
    {
        return Vector3(x * a, y * a, z * a);
    }

    void Vector3::operator+=(Vector3 v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
    }

    Vector3 Vector3::operator-(Vector3 v)
    {
        return Vec3(x - v.x, y - v.y, z - v.z);
    }

    const Vector3& Vector3::normalize()
    {
        T len = static_cast<T>(sqrt_asm(static_cast<double>(x * x + y * y + z * z)));
        x /= len;
        y /= len;
        z /= len;
        return *this;
    }

    T Vector3::lenSq()
    {
        return x * x + y * y + z * z;
    }
};

template <typename T>
struct Vector2 {
    T x = T(0);
    T y = T(0);
};

using Vector2i = Vector3<int>;
using Vector3f = Vector3<float>;
